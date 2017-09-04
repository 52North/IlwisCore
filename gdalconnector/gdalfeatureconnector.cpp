#include <QSqlQuery>
#include <QSqlError>
#include <fstream>
#include <iterator>

#include "coverage.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/Point.h"
#include "geos/geom/LineString.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/GeometryFactory.h"
#include "geos/geom/Coordinate.h"
#include "module.h"
#include "ilwiscontext.h"
#include "catalog.h"
#include "numericdomain.h"
#include "numericrange.h"
#include "table.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "textdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"
#include "gdalproxy.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "dataformat.h"
#include "gdalconnector.h"
#include "coverageconnector.h"
#include "juliantime.h"
#include "gdaltableloader.h"
#include "gdalfeatureconnector.h"

using namespace Ilwis;
using namespace Gdal;

GdalFeatureConnector::GdalFeatureConnector(const Resource &resource, bool load, const IOOptions &options) : CoverageConnector(resource,load, options){
}

ConnectorInterface* GdalFeatureConnector::create(const Resource &resource, bool load, const IOOptions &options) {
    return new GdalFeatureConnector(resource, load, options);
}

Ilwis::IlwisObject* GdalFeatureConnector::create() const{
    return new FeatureCoverage(this->_resource);
}

bool GdalFeatureConnector::loadMetaData(Ilwis::IlwisObject *data,const IOOptions& options){

    if(!CoverageConnector::loadMetaData(data, options))
        return false;

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    fcoverage->setFeatureCount(itFEATURE, iUNDEF, FeatureInfo::ALLFEATURES);

    OGRLayerH hLayer = getLayerHandle();

    if ( hLayer) {
        //feature types
        IlwisTypes type = GDALProxy::translateOGRType(gdal()->getLayerGeometry(hLayer));
        if (type == itUNKNOWN){
            WARN(QString("Unknown feature type of layer %1 from: %2").arg(0).arg(_fileUrl.toString()));
        }else{
            fcoverage->featureTypes(type);
        }

        //feature counts
        int temp = gdal()->getFeatureCount(hLayer, FALSE);//TRUE to FORCE databases to scan whole layer, FALSe can end up in -1 for unknown result
        if (temp == -1){
            WARN(QString("Couldn't determine feature count of layer %1 from meta data of %2").arg(0).arg(_fileUrl.toString()));
        }else{
            int featureCount = fcoverage->featureCount(type);
            featureCount += temp;
            fcoverage->setFeatureCount(type, featureCount,0); // subgeometries are not known at this level
        }
        //attribute table
        ITable attTable;
        Resource resource(_fileUrl, itTABLE);
        if(!attTable.prepare(resource,{"asflattable", true})) {//will load whole meta data of the table
            ERROR1(ERR_NO_INITIALIZED_1,resource.name());
            return false;
        }
        fcoverage->setAttributes(attTable);

        //layer envelopes/extents
        Envelope bbox;
        OGREnvelope envelope;//might sometimes be supported as 3D now only posssible from OGRGeometry
        OGRErr err = gdal()->getLayerExtent(hLayer, &envelope , FALSE);//TRUE to FORCE
        if (err != OGRERR_NONE){
            if (err == OGRERR_FAILURE){//on an empty layer or if simply too expensive(FORECE=FALSE) OGR_L_GetExtent may return OGRERR_FAILURE
                WARN(QString("Couldn't determine the extent of layer %1 from meta data of %2").arg(0).arg(_fileUrl.toString()));
            }else{
                ERROR0(QString("Couldn't load extent of layer %1 from %2: %3").arg(0).arg(_fileUrl.toString()).arg(gdal()->translateOGRERR(err)));
            }
        }else{
            bbox = Envelope(Coordinate(envelope.MinX,envelope.MinY),Coordinate(envelope.MaxX,envelope.MaxY));
        }
        fcoverage->envelope(bbox);
//        fcoverage->coordinateSystem()->envelope(bbox);
    }

    gdal()->closeFile(sourceRef().toLocalFile(), data->id());

    return true;
}

bool GdalFeatureConnector::loadData(IlwisObject* data, const IOOptions &){

    if(!GdalConnector::loadMetaData(data, IOOptions()))
        return false;

    bool ok = true;
    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    if ( fcoverage->isValid() ) {
        ITable attTable = fcoverage->attributeTable();
        if (!attTable.isValid()){
            ERROR2(ERR_NO_INITIALIZED_2,"attribute table",_fileUrl.toString());
            return false;
        }
        fcoverage->setFeatureCount(itFEATURE, iUNDEF, FeatureInfo::ALLFEATURES); // metadata already set it to correct number, creating new features will up the count agains; so reset to 0.

        OGRLayerH hLayer = getLayerHandle();
        if ( hLayer) {
            GdalTableLoader loader;
            attTable->dataLoaded(true); // new table, dont want any loading behaviour
            loader.setColumnCallbacks(attTable.ptr(), hLayer);
            std::vector<QVariant> record(attTable->columnCount());
            OGRFeatureH hFeature = 0;
            gdal()->resetReading(hLayer);
            //each FEATURE
            try {
                while( (hFeature = gdal()->getNextFeature(hLayer)) != NULL){
                    loader.loadRecord(attTable.ptr(), hFeature, record);
                    geos::geom::Geometry * geometry = fillFeature(fcoverage, gdal()->getGeometryRef(hFeature));
                    if (geometry){
                        auto feature = fcoverage->newFeature(geometry, false);
                        feature->record(record);
                    }else{
                        ERROR1("GDAL error during load of binary data: no geometry detected for feature in %1", _fileUrl.toString());
                    }
                    gdal()->destroyFeature( hFeature );
                }
            } catch (FeatureCreationError& ) {
                gdal()->destroyFeature( hFeature );
                ok = false;
            }
        }
        //layer envelopes/extents
        Envelope bbox;
        OGREnvelope envelope;//might sometimes be supported as 3D now only posssible from OGRGeometry
        OGRErr err = gdal()->getLayerExtent(hLayer, &envelope , TRUE);//TRUE to FORCE
        if (err != OGRERR_NONE && fcoverage->featureCount() != 0){
            ERROR0(QString("Couldn't load extent of a layer from %1 after binary was loaded: %2").arg(_fileUrl.toString()).arg(gdal()->translateOGRERR(err)));
        }else{
            bbox = Envelope(Coordinate(envelope.MinX,envelope.MinY),Coordinate(envelope.MaxX,envelope.MaxY));
        }
        fcoverage->envelope(bbox);
    }
    gdal()->closeFile(sourceRef().toLocalFile(), data->id());
    _binaryIsLoaded = ok;
    return ok;
}

geos::geom::Geometry* GdalFeatureConnector::fillFeature(FeatureCoverage *fcoverage, OGRGeometryH geometry ) const{
    if (geometry){
        OGRwkbGeometryType type = gdal()->getGeometryType(geometry);
        if (type == wkbPoint || type == wkbPoint25D){
            return fillPoint(fcoverage, geometry);
        }else if(type == wkbMultiPoint || type == wkbMultiPoint25D){
            return fillMultiPoint(fcoverage, geometry);
        }else if (type == wkbLineString || type == wkbLineString25D){
            return fillLine(fcoverage, geometry);
        }else if (type == wkbMultiLineString || type == wkbMultiLineString25D){
            return fillMultiLine(fcoverage, geometry);
        }else if (type == wkbPolygon || type == wkbPolygon25D){
            return fillPolygon(fcoverage, geometry);
        }else if (type == wkbMultiPolygon || type == wkbMultiPolygon25D){
            return fillMultiPolygon(fcoverage, geometry);
        }else{ // ( type == wkbGeometryCollection ){
            return fillGeometryCollection(fcoverage, geometry);
        }
    }else
        return 0;
}
geos::geom::Geometry* GdalFeatureConnector::fillPoint(FeatureCoverage *fcoverage, OGRGeometryH geometry ) const{
    double x,y,z;
    gdal()->getPoint(geometry, 0,&x,&y,&z);
    if (gdal()->getCoordinateDimension(geometry) == 2){
        Coordinate coord(x,y);
        return fcoverage->geomfactory()->createPoint(coord);
    }else if (gdal()->getCoordinateDimension(geometry) == 3){
        Coordinate coord(x,y,z);
        const UPGeomFactory& factory = fcoverage->geomfactory();
        return factory->createPoint(coord);
    }else{
        return fcoverage->geomfactory()->createPoint();//POINT EMPTY
    }
}
geos::geom::Geometry* GdalFeatureConnector::fillLine(FeatureCoverage *fcoverage, OGRGeometryH geometry ) const{
    int count = gdal()->getPointCount(geometry);
    if ( count == 0)
        return 0;

    geos::geom::CoordinateArraySequence *vertices = new geos::geom::CoordinateArraySequence(count);
    for(int i = 0; i < count; ++i){
        double x,y,z;
        gdal()->getPoint(geometry, i,&x,&y,&z);
        vertices->setAt(geos::geom::Coordinate(x,y), i);
    }
    return fcoverage->geomfactory()->createLineString(vertices);
}

geos::geom::Geometry* GdalFeatureConnector::fillPolygon(FeatureCoverage *fcoverage, OGRGeometryH geometry ) const{
    OGRGeometryH hSubGeometry = gdal()->getSubGeometryRef(geometry, 0);
    if(hSubGeometry){
        int count = gdal()->getPointCount(hSubGeometry);
        if ( count == 0)
            return 0;
        //OUTER POLYGON
        geos::geom::CoordinateArraySequence *outer = new geos::geom::CoordinateArraySequence(count);
        for(int i = 0; i < count; ++i) {
            double x,y,z;
            gdal()->getPoint(hSubGeometry, i,&x,&y,&z);
            outer->setAt(geos::geom::Coordinate(x,y),i);
        }
        geos::geom::LinearRing *outerring = fcoverage->geomfactory()->createLinearRing(outer);
        //INNER POLYGONS
        int subGeomCount = gdal()->getSubGeometryCount(geometry);
        std::vector<geos::geom::Geometry*> *inners = new std::vector<geos::geom::Geometry*>(subGeomCount - 1);
        for(int j = 1; j < subGeomCount; ++j) {
            hSubGeometry = gdal()->getSubGeometryRef(geometry, j);
            if(hSubGeometry){
                count = gdal()->getPointCount(hSubGeometry);
                if(count == 0)
                   continue;
                geos::geom::CoordinateArraySequence* innerring = new geos::geom::CoordinateArraySequence(count);
                for(int i = 0; i < count; ++i) {
                    double x,y,z;
                    gdal()->getPoint(hSubGeometry, i,&x,&y,&z);
                    innerring->setAt(geos::geom::Coordinate(x,y), i);
                }
                (*inners)[j-1] = fcoverage->geomfactory()->createLinearRing(innerring);
            }
        }
        return fcoverage->geomfactory()->createPolygon(outerring, inners);
    }else{
        ERROR1("GDAL couldn't find outer ring of a polygon for a record in %1",_fileUrl.toString());
        return 0;
    }
}

geos::geom::Geometry* GdalFeatureConnector::fillMultiPoint(FeatureCoverage *fcoverage, OGRGeometryH geometry ) const{
    int subGeomCount = gdal()->getSubGeometryCount(geometry);
    if(subGeomCount > 0){
        std::vector<geos::geom::Geometry *>* multi = new std::vector<geos::geom::Geometry *>();
        for(int i = 0; i < subGeomCount; ++i) {
            OGRGeometryH hSubGeometry = gdal()->getSubGeometryRef(geometry, i);
            if(hSubGeometry){
                multi->push_back(fillPoint(fcoverage, hSubGeometry));
            }
        }
        return fcoverage->geomfactory()->createMultiPoint(multi);
    }else
        return 0;
}

geos::geom::Geometry* GdalFeatureConnector::fillMultiLine(FeatureCoverage *fcoverage, OGRGeometryH geometry ) const{
    int subGeomCount = gdal()->getSubGeometryCount(geometry);
    if(subGeomCount > 0){
        std::vector<geos::geom::Geometry *>* multi = new std::vector<geos::geom::Geometry *>();
        for(int i = 0; i < subGeomCount; ++i) {
            OGRGeometryH hSubGeometry = gdal()->getSubGeometryRef(geometry, i);
            if(hSubGeometry){
                multi->push_back(fillLine(fcoverage, hSubGeometry));
            }
        }
        return fcoverage->geomfactory()->createMultiLineString(multi);
    }else
        return 0;
}

geos::geom::Geometry* GdalFeatureConnector::fillMultiPolygon(FeatureCoverage *fcoverage, OGRGeometryH geometry ) const{
    int subGeomCount = gdal()->getSubGeometryCount(geometry);
    if(subGeomCount > 0){
       std::vector<geos::geom::Geometry *>* multi = new std::vector<geos::geom::Geometry *>();
        for(int i = 0; i < subGeomCount; ++i) {
            OGRGeometryH hSubGeometry = gdal()->getSubGeometryRef(geometry, i);
            if(hSubGeometry){
                multi->push_back(fillPolygon(fcoverage, hSubGeometry));
            }
        }
        return fcoverage->geomfactory()->createMultiPolygon(multi);
    }else
        return 0;
}

geos::geom::Geometry* GdalFeatureConnector::fillGeometryCollection(FeatureCoverage *fcoverage, OGRGeometryH geometry ) const{
    int subGeomCount = gdal()->getSubGeometryCount(geometry);
    if(subGeomCount){
        std::vector<geos::geom::Geometry *>* multi = new std::vector<geos::geom::Geometry *>();
        for(int i = 0; i < subGeomCount; ++i) {
            OGRGeometryH hSubGeometry = gdal()->getSubGeometryRef(geometry, i);
            if(hSubGeometry){
                multi->push_back(fillFeature(fcoverage, hSubGeometry));
            }
        }
        return fcoverage->geomfactory()->createGeometryCollection(multi);
    }else
        return 0;
}


OGRDataSourceH GdalFeatureConnector::createFileBasedDataSource(const QString& postfix, const QFileInfo& fileinfo) const {
    QString outputname = fileinfo.absolutePath() + "/" + fileinfo.baseName() + postfix + "." + fileinfo.suffix();
    if ( QFileInfo(outputname).exists()){
        if ( remove(outputname.toLocal8Bit())!= 0){
            //if(!QFile::remove(outputname)) {
                ERROR1(ERR_COULDNT_CREATE_OBJECT_FOR_1,outputname);
                //return 0;
            }
    }
    OGRDataSourceH datasource = gdal()->createDatasource(_driver, outputname.toLocal8Bit(),0);
    if ( datasource == 0) {
        ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "data source",fileinfo.fileName());
        return 0;
    }

    return datasource;
}

OGRLayerH GdalFeatureConnector::createLayer(const QString& name,OGRwkbGeometryType type,OGRSpatialReferenceH srs, OGRDataSourceH  source ) {
    if ( source == 0)
        return 0;
    OGRLayerH layer = gdal()->createOgrLayer(source, name.toLocal8Bit(),srs,type, 0);
    if ( layer == 0){
        ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "layer",name);
        return 0;
    }
    return layer;
}

bool GdalFeatureConnector::createAttributes(const ITable& tbl, OGRLayerH layer, const std::vector<OGRFieldDefnH>& fielddefs,std::vector<bool>& validAttributes) {
    if ( layer == 0)
        return false;

    int index=0;
    for(int i=0; i < tbl->columnCount(); ++i){
        if ( validAttributes[i]) {
            if(gdal()->addAttribute(layer,fielddefs[index],TRUE) != OGRERR_NONE){
                validAttributes[i] = false;
                WARN2(ERR_NO_INITIALIZED_2,tbl->columndefinition(i).name(),tbl->name());
            }
            ++index;
        }
    }
    return true;
}

OGRwkbGeometryType GdalFeatureConnector::ilwisType2GdalFeatureType(IlwisTypes tp) {
    switch (tp) {
    case itPOINT:
        return wkbPoint;
    case itLINE:
        return wkbLineString;
    case itPOLYGON:
        return wkbPolygon;
    }
    return wkbUnknown;
}

void GdalFeatureConnector::setAttributes(OGRFeatureH hfeature, SPFeatureI& feature, const std::vector<bool>& validAttributes, const std::vector<ColumnDefinition>& columnDef) {
    int index = 0;
    for(int i=0; i < feature->attributeColumnCount(); ++i){
        if ( !validAttributes[i])
            continue;
        IDomain dom = columnDef[i].datadef().domain<>();
        if(hasType(dom->valueType(),itINTEGER)) {
            gdal()->setIntegerAttribute(hfeature,index,feature->cell(i).toInt());
        } else if (hasType(dom->valueType(),itDOUBLE | itFLOAT)) {
            gdal()->setDoubleAttribute(hfeature,index,feature->cell(i).toDouble());
        } else if (hasType(dom->valueType(),itTHEMATICITEM | itNAMEDITEM | itINDEXEDITEM | itNUMERICITEM | itTIMEITEM)) {
            gdal()->setStringAttribute(hfeature,index,dom->impliedValue(feature->cell(i)).toString().toLocal8Bit());
        } else if (hasType(dom->valueType(), itDATETIME)) {
            QVariant v = feature->cell(i);
            if ( QString(v.typeName()).compare("Ilwis::Time") != 0){
                ERROR2(ERR_COULD_NOT_CONVERT_2,v.toString(), "time");
                gdal()->setDateTimeAttribute(hfeature,index,0,0,0,0,0,0,0);
            }else{
                Time time = v.value<Ilwis::Time>();
                gdal()->setDateTimeAttribute(hfeature,index,
                                             time.get(Time::tpYEAR),
                                             time.get(Time::tpMONTH),
                                             time.get(Time::tpDAYOFMONTH),
                                             time.get(Time::tpHOUR),
                                             time.get(Time::tpMINUTE),
                                             time.get(Time::tpSECOND),
                                             0);//TODO TimeZone??
            }
        } else if (hasType(dom->valueType(),itSTRING)){
            gdal()->setStringAttribute(hfeature,index,feature->cell(i).toString().toLocal8Bit());
        }
        ++index;
    }

}

bool GdalFeatureConnector::createDataSourceAndLayers(IlwisTypes types,
                                                     const QString& postfix,
                                                     const IFeatureCoverage& features,
                                                     OGRSpatialReferenceH srs,
                                                     const std::vector<OGRFieldDefnH>& fielddefs,
                                                     std::vector<SourceHandles>& datasources,
                                                     std::vector<bool>& validAttributes ){

    QFileInfo fileinfo = sourceRef().toLocalFile();
    int typeIndex  = ilwisType2Index(types);
    ITable tbl = features->attributeTable();
    datasources[typeIndex]._source =  createFileBasedDataSource(postfix, fileinfo);
    if ( datasources[typeIndex]._source == 0)
        return false;
    datasources[typeIndex]._layers.push_back(createLayer(features->name(), ilwisType2GdalFeatureType(types), srs,datasources[typeIndex]._source));
    if (  datasources[typeIndex]._layers.back() == 0)
        return false;

    return createAttributes(tbl, datasources[typeIndex]._layers[0], fielddefs,validAttributes);
}

bool GdalFeatureConnector::setDataSourceAndLayers(const IFeatureCoverage& features, std::vector<SourceHandles>& datasources,std::vector<bool>& validAttributes) {


    ITable tbl = features->attributeTable();
    validAttributes.resize(tbl->columnCount(), false);
    std::vector<OGRFieldDefnH> fielddefs(tbl->columnCount());

    int index = 0;
    for(int i=0; i < tbl->columnCount(); ++i){
        OGRFieldType ogrtype = ilwisType2GdalFieldType(tbl->columndefinition(i).datadef().domain<>()->valueType());
        OGRFieldDefnH fieldef = gdal()->createAttributeDefintion(tbl->columndefinition(i).name().toLocal8Bit(),ogrtype);
        if ( fieldef == 0){
            WARN2(ERR_INVALID_INIT_FOR_2, TR("data-type"), tbl->columndefinition(i).name());
        }else
            validAttributes[i] = true;

        fielddefs[index++] = fieldef;
    }
    bool ok = false;
    OGRSpatialReferenceH srs = createSRS(features->coordinateSystem());
    IlwisTypes types = features->featureTypes();
    bool multipleoutputs = (types == (itPOINT | itLINE)) || (types == (itPOINT | itPOLYGON)) || (types == (itLINE | itPOLYGON)) || (types == (itFEATURE));
    if ( multipleoutputs){
        if ((features->featureTypes() & itPOINT) != 0) {
            ok = createDataSourceAndLayers(itPOINT, "point", features, srs,fielddefs,datasources,validAttributes);
        }
        if ((features->featureTypes() & itLINE) != 0) {
            ok = createDataSourceAndLayers(itLINE, "line", features, srs,fielddefs,datasources,validAttributes);
        }
        if ((features->featureTypes() & itPOLYGON) != 0) {
            ok = createDataSourceAndLayers(itPOLYGON, "polygon", features, srs,fielddefs,datasources,validAttributes);
        }
    }else {
        ok = createDataSourceAndLayers(types, "", features, srs,fielddefs,datasources,validAttributes);
    }

    for(OGRFieldDefnH fieldef : fielddefs) {
        gdal()->destroyAttributeDefintion(fieldef);
    }
    return ok;
}

int GdalFeatureConnector::ilwisType2Index(IlwisTypes tp)
{
    switch (tp){
        case itPOINT:
            return 0;
        case itLINE:
            return 1;
        case itPOLYGON:
            return 2;
    }
    return itUNKNOWN;
}

OGRGeometryH GdalFeatureConnector::createFeature(const geos::geom::Geometry* geom){
    geos::geom::GeometryTypeId type = geom->getGeometryTypeId();
    OGRGeometryH hgeom = 0;
    switch (type){
        case geos::geom::GEOS_POINT: hgeom = createPoint(geom); break;
        case geos::geom::GEOS_LINESTRING: hgeom = createLine(geom); break;
        case geos::geom::GEOS_POLYGON: hgeom = createPolygon(geom); break;
        case geos::geom::GEOS_MULTIPOINT: hgeom = createMultiPoint(geom); break;
        case geos::geom::GEOS_MULTILINESTRING: hgeom = createMultiLine(geom); break;
        case geos::geom::GEOS_MULTIPOLYGON: hgeom = createMultiPolygon(geom); break;
        default: hgeom = createGeometryCollection(geom);
    }
    return hgeom;
}

OGRGeometryH GdalFeatureConnector::createPoint(const geos::geom::Geometry* geom){
    OGRGeometryH hpoint = 0;
    if (geom->getCoordinateDimension() == 2)
        hpoint = gdal()->createGeometry(wkbPoint);
    else
        hpoint = gdal()->createGeometry(wkbPoint25D);

    const geos::geom::Coordinate* crd = geom->getCoordinate();
    if (crd != NULL){
        Coordinate coord(*crd);
        if (coord.is3D())
            gdal()->addPoint(hpoint, coord.x, coord.y, coord.z);
        else
            gdal()->addPoint2D(hpoint, coord.x, coord.y);
    }
    return hpoint;
}

OGRGeometryH GdalFeatureConnector::createLine(const geos::geom::Geometry* geom){
    OGRGeometryH hline = 0;
    if (geom->getCoordinateDimension() == 2){
        hline = gdal()->createGeometry(wkbLineString);
    }else{
        hline = gdal()->createGeometry(wkbLineString25D);
    }
    const geos::geom::LineString* line = dynamic_cast<const geos::geom::LineString*>(geom);
    if(line){
        const geos::geom::CoordinateSequence* coords = line->getCoordinatesRO();
        if (coords != NULL){
            const std::vector<geos::geom::Coordinate>* vec = coords->toVector();
            for(auto it = vec->begin(); it != vec->end(); it++) {
                Coordinate coord = (*it);
                if (coord.is3D())
                    gdal()->addPoint(hline, coord.x, coord.y, coord.z);
                else
                    gdal()->addPoint2D(hline, coord.x, coord.y);
            }
        }
    }
    return hline;
}

OGRGeometryH GdalFeatureConnector::createPolygon(const geos::geom::Geometry* geom){
    OGRGeometryH hpolygon = 0;
    if (geom->getCoordinateDimension() == 2){
        hpolygon = gdal()->createGeometry(wkbPolygon);
    }else{
        hpolygon = gdal()->createGeometry(wkbPolygon25D);
    }
    const geos::geom::Polygon* polygon = dynamic_cast<const geos::geom::Polygon *>(geom);
    if(polygon){
        OGRGeometryH hring = gdal()->createGeometry(wkbLinearRing);
        //ExteriorRing
        const geos::geom::CoordinateSequence* coords = polygon->getExteriorRing()->getCoordinatesRO();
        if (coords != NULL){
            const std::vector<geos::geom::Coordinate>* vec = coords->toVector();
            for(auto it = vec->begin(); it != vec->end(); it++) {
                Coordinate coord = (*it);
                if (coord.is3D())
                    gdal()->addPoint(hring, coord.x, coord.y, coord.z);
                else
                    gdal()->addPoint2D(hring, coord.x, coord.y);
            }
        }
        gdal()->addGeometryDirectly(hpolygon, hring);
        //InteriorRings
        for(int hole=0; hole < polygon->getNumInteriorRing(); ++hole){
            hring = gdal()->createGeometry(wkbLinearRing);
            const geos::geom::CoordinateSequence* coords = polygon->getInteriorRingN(hole)->getCoordinatesRO();
            if (coords != NULL){
                const std::vector<geos::geom::Coordinate>* vec = coords->toVector();
                for(auto it = vec->begin(); it != vec->end(); it++) {
                    Coordinate coord = (*it);
                    if (coord.is3D())
                        gdal()->addPoint(hring, coord.x, coord.y, coord.z);
                    else
                        gdal()->addPoint2D(hring, coord.x, coord.y);
                }
            }
            gdal()->addGeometryDirectly(hpolygon, hring);
        }
    }
    return hpolygon;
}

OGRGeometryH GdalFeatureConnector::createMultiPoint(const geos::geom::Geometry* geom){
    OGRGeometryH hmulti = gdal()->createGeometry(wkbMultiPoint);
    OGRErr err = OGRERR_NONE;
    for(int i = 0; i < geom->getNumGeometries();i++){
        err = gdal()->addGeometryDirectly(hmulti, createPoint(geom->getGeometryN(i)));
        if(err != OGRERR_NONE)
            ERROR1("not able to add point to non-container geometry in %1",_fileUrl.toString());
    }
    return hmulti;
}

OGRGeometryH GdalFeatureConnector::createMultiLine(const geos::geom::Geometry* geom){
    OGRGeometryH hmulti = gdal()->createGeometry(wkbMultiLineString);
    OGRErr err = OGRERR_NONE;
    for(int i = 0; i < geom->getNumGeometries();i++){
        err = gdal()->addGeometryDirectly(hmulti, createLine(geom->getGeometryN(i)));
        if(err != OGRERR_NONE)
            ERROR1("not able to add line to non-container geometry in %1",_fileUrl.toString());
    }
    return hmulti;

}

OGRGeometryH GdalFeatureConnector::createMultiPolygon(const geos::geom::Geometry* geom){
    OGRGeometryH hmulti = gdal()->createGeometry(wkbMultiPolygon);
    OGRErr err = OGRERR_NONE;
    for(int i = 0; i < geom->getNumGeometries();i++){
        err = gdal()->addGeometryDirectly(hmulti, createPolygon(geom->getGeometryN(i)));
        if(err != OGRERR_NONE)
            ERROR1("not able to add polygon to non-container geometry in %1",_fileUrl.toString());
    }
    return hmulti;
}

OGRGeometryH GdalFeatureConnector::createGeometryCollection(const geos::geom::Geometry* geom){
    OGRGeometryH hmulti = gdal()->createGeometry(wkbGeometryCollection);
    OGRErr err = OGRERR_NONE;
    for(int i = 0; i < geom->getNumGeometries();i++){
        err = gdal()->addGeometryDirectly(hmulti, createPolygon(geom->getGeometryN(i)));
        if(err != OGRERR_NONE)
            ERROR1("not able to add geometry to non-container geometry in %1",_fileUrl.toString());
    }
    return hmulti;
}

bool GdalFeatureConnector::loadDriver()
{

    _driver = gdal()->getDriverByName(_gdalShortName.toLocal8Bit());
    if ( !_driver ) {
        return ERROR2(ERR_COULD_NOT_LOAD_2, "data-source", _fileUrl.toString());
    }


    return true;
}

bool GdalFeatureConnector::store(IlwisObject *obj, const IOOptions&   )
{

    if (!loadDriver())
        return false;

    IFeatureCoverage coverage;
    coverage.set(static_cast<FeatureCoverage *>(obj));

    std::vector<SourceHandles> datasources(3);
    std::vector<bool> validAttributes;
    if(!setDataSourceAndLayers(coverage, datasources, validAttributes))
        return false;

    std::vector<ColumnDefinition> defs;
    for(int i=0; i < coverage->attributeTable()->columnCount(); ++i){
        defs.push_back(coverage->attributeTable()->columndefinition(i));
    }

    FeatureIterator fiter(coverage);
    FeatureIterator endIter = end(coverage);
    for(; fiter != endIter; ++fiter) {
        SPFeatureI feature = *fiter;
        IlwisTypes geomType = feature->geometryType();
        if ( geomType != 0){
            OGRLayerH lyr = datasources[ilwisType2Index(geomType)]._layers[0];
            OGRFeatureH hfeature = gdal()->createFeature(gdal()->getLayerDef(lyr));
            if (hfeature) {
                setAttributes(hfeature, feature, validAttributes, defs);
                const geos::geom::Geometry* geometry = feature->geometry().get();
                if (gdal()->setGeometryDirectly(hfeature,createFeature(geometry)) != OGRERR_NONE)
                    ERROR2(ERR_COULD_NOT_ALLOCATE_2, TR("geometry"), _fileUrl.toString());
                if (gdal()->addFeature2Layer(lyr, hfeature) != OGRERR_NONE) {
                    ERROR2(ERR_COULD_NOT_ALLOCATE_2, TR("feature"), _fileUrl.toString());
                }
                gdal()->destroyFeature(hfeature);
            };
        }
    }
    for(auto& datasource : datasources){
        if ( datasource._source != 0)
            gdal()->destroyDataSource(datasource._source);
    }

    return true;

}
