#include "kernel.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "coordinatesystem.h"
#include "proj4parameters.h"
#include "conventionalcoordinatesystem.h"

using namespace Ilwis;

ConventionalCoordinateSystem::ConventionalCoordinateSystem() : _unit(sUNDEF)
{

}

ConventionalCoordinateSystem::ConventionalCoordinateSystem(const Resource &resource) : CoordinateSystem(resource),_unit(sUNDEF)
{
}

ConventionalCoordinateSystem::~ConventionalCoordinateSystem()
{
    _projection.set(0);
    _ellipsoid.set(0);
}

Coordinate ConventionalCoordinateSystem::coord2coord(const ICoordinateSystem &sourceCs, const Coordinate &crdSource) const
{
    //TODO isLatLon guard doesn't consider latlon cs other than WGS84!
    if (sourceCs->id() == id()) //  the 'real'isEqual test is too expensive here, as this method can be called 100000's of times (resample)
        return crdSource;
    LatLon ll = sourceCs->isLatLon() ? LatLon(crdSource.y,crdSource.x) : sourceCs->coord2latlon(crdSource);
    if ( ll.isValid()) {
        return isLatLon() ? ll : latlon2coord(ll);
    }
    return Coordinate();
}

LatLon ConventionalCoordinateSystem::coord2latlon(const Coordinate &crdSource) const
{
    LatLon pl = _projection->coord2latlon(crdSource);
    if (!pl.isValid())
        return llUNDEF;
    if (abs(pl.lon()) > 180)
        return llUNDEF;
    return pl;
}

Coordinate ConventionalCoordinateSystem::latlon2coord(const LatLon &ll) const
{
    Coordinate xy = _projection->latlon2coord(ll);
    if (xy == crdUNDEF)
        return crdUNDEF;
    return xy;

}

bool ConventionalCoordinateSystem::isEqual(const IlwisObject *obj) const
{
    if ( !obj || !hasType(obj->ilwisType(), itCONVENTIONALCOORDSYSTEM))
        return false;

    if(id() == obj->id())
        return true;

    const ConventionalCoordinateSystem *csy = static_cast<const ConventionalCoordinateSystem *>(obj);
    if ( !csy->isValid())
        return false;

    bool equal = true;
    if ( csy->projection().isValid() && projection().isValid()) {
        equal = (csy->projection()->code() == "longlat" && projection()->code() == "longlat") || csy->projection()->isEqual(projection().ptr());
    }
    if ( datum() && csy->datum()) {
        equal &= (datum()->code() == csy->datum()->code());
    } else if (ellipsoid().isValid() && csy->ellipsoid().isValid()) {
        equal &= ellipsoid()->isEqual(csy->ellipsoid());
    }
    return equal;
}

bool ConventionalCoordinateSystem::isUnknown() const
{
    return false;
}

bool ConventionalCoordinateSystem::canConvertToLatLon() const
{
    if ( _projection.isValid())    {
        return _projection->canConvertToLatLon();
    }
    return false;
}

bool ConventionalCoordinateSystem::canConvertToCoordinate() const
{
    if ( _projection.isValid())    {
        return _projection->canConvertToCoordinate();
    }
    return false;
}

QString ConventionalCoordinateSystem::toWKT(quint32 spaces) const
{
    QString wkt = "PROJCS[\"" + name() + "\"" + ",";
    QString geocs, proj, ell, datum;
    geocs += QString(" ").repeated(spaces);
    QString ending = spaces == 0 ? "" : "\n";

    geocs += "GEOCS[\"";
    if ( name().indexOf("/") != -1){
        geocs += name().left( name().indexOf("/")).trimmed() + "\"";
    }else
        geocs += name() + "\"";
    if ( _datum)
        datum = _datum->toWKT(spaces * 2);
    else
        datum += "DATUM[\"Unknown Datum" ;
    QString primm = "PRIMEM[\"Greenwich\",0, AUTHORITY[\"EPSG\",8901\"]]";
    if ( _ellipsoid.isValid()){
        ell = _ellipsoid->toWKT(spaces * 2) + ",";
    }
    if ( _projection.isValid())
        proj= _projection->toWKT(spaces * 2) + ",";

    if ( geocs != ""){
        wkt += geocs + (datum !="" ? "," + datum : "") + "," + ell + primm + "]," + ending;
    }else
        wkt += ell + ",";
    wkt += proj;

    if ( _unit != sUNDEF){
        wkt += "UNIT[" + _unit + ",1.0]";
    }else
        wkt += "UNIT[\"meter\",1.0]";

    return wkt + "]";
}

QString ConventionalCoordinateSystem::toProj4() const
{
    if ( this->projection().isValid())
        return this->projection()->toProj4();
    else
        return QString("?");
}

QString ConventionalCoordinateSystem::unit() const
{
    return _unit;
}

void ConventionalCoordinateSystem::unit(const QString& unit)
{
    _unit = unit;
}

bool ConventionalCoordinateSystem::isCompatibleWith(const IlwisObject *obj, bool strict) const
{
    if ( projection().isValid() && obj->isValid()){
        const ConventionalCoordinateSystem *ccsy = dynamic_cast<const ConventionalCoordinateSystem *>(obj);
        if ( ccsy)
            return projection()->code() == ccsy->projection()->code();
    }
    return false;
}

IlwisObject *ConventionalCoordinateSystem::clone()
{
    ConventionalCoordinateSystem *csy = new ConventionalCoordinateSystem();
    copyTo(csy);
    return csy;
}

void ConventionalCoordinateSystem::copyTo(IlwisObject *obj)
{
    Locker<> lock(_mutex);
    CoordinateSystem::copyTo(obj);
    ConventionalCoordinateSystem *csy = static_cast<ConventionalCoordinateSystem *>(obj);
    csy->_ellipsoid.set( static_cast<Ellipsoid *>(_ellipsoid->clone()));
    csy->_projection.set( static_cast<Projection *>(projection()->clone()));
    csy->projection()->setCoordinateSystem(this);
}

const std::unique_ptr<GeodeticDatum>& ConventionalCoordinateSystem::datum() const
{
    return _datum;
}

void ConventionalCoordinateSystem::setDatum(GeodeticDatum *datum)
{
    _datum.reset(datum);
}

IEllipsoid ConventionalCoordinateSystem::ellipsoid() const
{
    return _ellipsoid;
}

void ConventionalCoordinateSystem::setEllipsoid(const IEllipsoid &ell)
{
    _ellipsoid = ell;
    if ( _projection.isValid() && ell.isValid()){
        _projection->setParameter(Projection::pvELLCODE,ell->toProj4());
    }
}

bool ConventionalCoordinateSystem::isLatLon() const
{
    return _unit == "degrees";
}

IlwisTypes ConventionalCoordinateSystem::ilwisType() const
{
    return itCONVENTIONALCOORDSYSTEM;
}

bool ConventionalCoordinateSystem::isValid() const
{
    bool ok1 =  _projection.isValid();
    bool ok2 =_ellipsoid.isValid();

    return ok1 && ok2;
}



void ConventionalCoordinateSystem::setProjection(const IProjection &proj)
{
    _projection = proj;
    if ( proj->code().contains("longlat") || proj->code().contains("latlon"))
        _unit = "degrees";
}

IProjection ConventionalCoordinateSystem::projection() const
{
    return _projection;
}

bool ConventionalCoordinateSystem::prepare(const IOOptions& options)
{
    return CoordinateSystem::prepare(options);
}

// TODO: this method should be merged with the prepare above
bool ConventionalCoordinateSystem::prepare(const QString &parms)
{
    Proj4Parameters proj4(parms);

    QString ell = proj4["ellps"];
    if ( ell != sUNDEF) {
        _ellipsoid.prepare("code=ellipsoid:" + ell);
    } else {
        _ellipsoid.prepare();
        QString laxis = proj4["a"];
        if ( laxis != sUNDEF) {
            QString saxis = proj4["b"];
            bool ok;
            double a = laxis.toDouble(&ok);
            if (!ok) return ERROR2(ERR_INVALID_PROPERTY_FOR_2, "ellipsoid", name());
            double b = saxis.toDouble(&ok);
            if (!ok) return ERROR2(ERR_INVALID_PROPERTY_FOR_2, "ellipsoid", name());

            double f = (a - b) / a;
            _ellipsoid->setEllipsoid(a, f);
        } else
            _ellipsoid->setEllipsoid(6378137.0, 298.257223563);
    }
    if ( proj4.hasDatum()) {
        _datum.reset(new GeodeticDatum());
        if ( proj4["dx"] != sUNDEF) {
            if (proj4["rx"] != sUNDEF) { // 7 parameter version
                _datum->set7TransformationParameters(proj4["dx"].toDouble(),
                                                     proj4["dy"].toDouble(),
                                                     proj4["dz"].toDouble(),
                                                     proj4["rx"].toDouble(),
                                                     proj4["ry"].toDouble(),
                                                     proj4["rz"].toDouble(),
                                                     proj4["dscale"].toDouble());
            } else { // 3 parameter version
                _datum->set3TransformationParameters(proj4["dx"].toDouble(),
                                                     proj4["dy"].toDouble(),
                                                     proj4["dz"].toDouble(),
                                                     _ellipsoid->name());
            }
        } else if ( proj4["datum"] != sUNDEF) {
            _datum->fromCode(proj4["datum"]);
        }
    }
    QString code = extractProjection(proj4);

    if ( code == sUNDEF) {
        kernel()->issues()->log(TR(ERR_INVALID_PROPERTY_FOR_2).arg("projection name", name()));
        return false;
    }
    code = "code=proj4: " + code;
    Resource prj = mastercatalog()->name2Resource(code, itPROJECTION);
    if (!prj.isValid()) {
        prj = Resource("ilwis://projection/" + code, itPROJECTION);
        prj.code(proj4["proj"]); // set the code to the projection "CODE", as in projections.csv
        mastercatalog()->addItems({prj});
    }
    IOOptions opt;
    opt << IOOptions::Option("proj4",parms.mid(6));
    bool ok =_projection.prepare(prj.id(),opt);
    _projection->setCoordinateSystem(this);
    if ( _projection.isValid())  {
        ok = _projection->prepare(parms);
    }
    QString unit = proj4["units"];
    if ( ok && (_projection->code().contains("longlat") || _projection->code().contains("latlon") || unit=="?"))
        _unit = "degrees";
    else{
        if ( unit == "m")
            _unit = "meter";
        else
            _unit = "feet";
    }


    return ok;
}

QString ConventionalCoordinateSystem::extractProjection(const Proj4Parameters & proj4) const
{
    QString proj = "";
    // The elements that make the projection unique (the projection parameters). Excluding datum and ellipsoid.
    std::vector<QString> elements = {"proj", "lat_0", "lat_1", "lat_2", "lon_0", "x_0", "y_0", "zone", "south", "k", "k_0", "h"}; // lon_1 ?
    for (QString element : elements) { // fixed order
        QString val = proj4[element];
        if (val != sUNDEF) {
            if (proj.length() > 0)
                proj += " ";
            proj += "+" + element;
            if (val.length() > 0)
                proj += "=" + val;
        }
    }
    if (proj.length() == 0)
        proj = sUNDEF;
    return proj;
}
