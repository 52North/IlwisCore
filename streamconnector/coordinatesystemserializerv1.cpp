#include "kernel.h"
#include "version.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "projection.h"
#include "geodeticdatum.h"
#include "coordinatesystem.h"
#include "proj4parameters.h"
#include "conventionalcoordinatesystem.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "streamconnector.h"
#include "versionedserializer.h"
#include "versioneddatastreamfactory.h"
#include "coordinatesystemserializerv1.h"


using namespace Ilwis;
using namespace Stream;

VersionedSerializer *CoordinateSystemSerializerV1::create( QDataStream& stream)
{
    return new CoordinateSystemSerializerV1(stream);
}

CoordinateSystemSerializerV1::CoordinateSystemSerializerV1(QDataStream& stream) : VersionedSerializer(stream)
{
}

bool CoordinateSystemSerializerV1::store(IlwisObject *obj, const IOOptions &options)
{
    if (!VersionedSerializer::store(obj, options))
        return false;

    CoordinateSystem *csy = static_cast<CoordinateSystem *>(obj);
    if ( csy->ilwisType() == itCONVENTIONALCOORDSYSTEM){
        VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
        if (!factory)
            return false;
        std::unique_ptr<DataInterface> projstreamer(factory->create(Version::interfaceVersion, itPROJECTION,_stream));
        if ( !projstreamer)
            return false;
        ConventionalCoordinateSystem *ccsy = static_cast<ConventionalCoordinateSystem *>(csy);
        storeSystemPath(ccsy->projection()->resource());
        projstreamer->store(ccsy->projection().ptr(),options);
        std::unique_ptr<DataInterface> ellstreamer(factory->create(Version::interfaceVersion, itELLIPSOID,_stream));
        if ( !ellstreamer)
            return false;
        storeSystemPath(ccsy->ellipsoid()->resource());
        ellstreamer->store(ccsy->ellipsoid().ptr(),options);

        const std::unique_ptr<GeodeticDatum>& datum = ccsy->datum();
        if ( datum){
            _stream << itGEODETICDATUM << datum->name() << datum->code() << datum->description() << datum->area() << datum->authority();
            for(int i =0; i < 10; ++i)
                _stream << datum->parameter((GeodeticDatum::DatumParameters)i);
        }else
            _stream << itUNKNOWN;

        _stream << ccsy->unit();

    }
    _stream << csy->envelope().min_corner().x << csy->envelope().min_corner().y << csy->envelope().max_corner().x << csy->envelope().max_corner().y;
    return true;

}

bool CoordinateSystemSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!VersionedSerializer::loadMetaData(obj, options))
        return false;
    CoordinateSystem *csy = static_cast<CoordinateSystem *>(obj);
    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;

    if ( csy->ilwisType() == itCONVENTIONALCOORDSYSTEM){
        ConventionalCoordinateSystem *convCsy = static_cast<ConventionalCoordinateSystem *>(obj);
        quint64 type;
        QString version, url;
        _stream >> url;
        _stream >> type;
        if ( type != itUNKNOWN){
            _stream >> version;
            std::unique_ptr<DataInterface> projstreamer(factory->create(version, type,_stream));
            if ( !projstreamer)
                return false;
            IProjection proj(itPROJECTION);
            projstreamer->loadMetaData(proj.ptr(), options );
            convCsy->setProjection(proj);
        }
        _stream >> url;
        _stream >> type;
        if ( type != itUNKNOWN){
            _stream >> version;

            std::unique_ptr<DataInterface> ellstreamer(factory->create(Version::interfaceVersion, itELLIPSOID,_stream));
            if ( !ellstreamer)
                return false;
            IEllipsoid systemEll = makeSystemObject<IEllipsoid>(url);
            IEllipsoid ellipsoid;
            ellipsoid.prepare();
            ellstreamer->loadMetaData(ellipsoid.ptr(), options);
            convCsy->setEllipsoid(systemEll.isValid() ? systemEll : ellipsoid);
        }
        _stream >> type;
        if ( type != itUNKNOWN){
            QString name, code, description, area, authority;
            _stream >> name >> code >> description >> area >> authority;
            GeodeticDatum *datum = new GeodeticDatum(name);
            datum->setArea(area);
            datum->code(code);
            datum->setDescription(description);
            datum->setAuthority(authority);
            double values[10];
            for(int i =0; i < 10; ++i){
                _stream >> values[i];
            }
            datum->set10TransformationParameters(values[0],values[1], values[2], values[3], values[4], values[5], values[6],Ilwis::Coordinate(values[7], values[8], values[9]));
            convCsy->setDatum(datum);

        }
        QString unit;
        _stream >> unit;
        convCsy->unit(unit);

    }
    double minx, miny, maxx, maxy;
    _stream >> minx >> miny >> maxx >> maxy;
    csy->envelope(Envelope(Coordinate(minx, miny), Coordinate(maxx, maxy)));

    return true;
}
