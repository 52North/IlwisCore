#include <QString>
#include <functional>

#include "kernel.h"
#include "ilwis.h"
#include "geos/geom/Coordinate.h"
#include "coordinate.h"
#include "location.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "projectionimplementation.h"
#include "proj_api.h"
#include "prjimplproj4.h"
#include "proj4parameters.h"
#include "box.h"
#include "coordinatesystem.h"
#include "conventionalcoordinatesystem.h"


using namespace Ilwis;

ProjectionImplementation *ProjectionImplementationProj4::create(const Resource& resource) {
    return new ProjectionImplementationProj4(resource);
}

void ProjectionImplementationProj4::setParameter(Projection::ProjectionParamValue type, const QVariant &v)
{
    ProjectionImplementation::setParameter(type, v);
    QString value = v.toString();
    switch(type) {
    case Projection::pvFALSEEASTING:
        _targetDef += " +x_0=" + value;break;
    case Projection::pvFALSENORTHING:
        _targetDef += " +y_0=" + value; break;
    case Projection::pvCENTRALMERIDIAN:
        _targetDef += " +lon_0=" + value; break;
    case Projection::pvCENTRALPARALLEL:
        _targetDef += " +lat_0=" + value; break;
    case Projection::pvSTANDARDPARALLEL1:
        _targetDef += " +lat_1=" + value; break;
    case Projection::pvSTANDARDPARALLEL2:
        _targetDef += " +lat_2=" + value; break;
    case Projection::pvLATITUDEOFTRUESCALE:
        _targetDef += " +lat_ts=" + value; break;
    case Projection::pvZONE:
        _targetDef += " +zone=" + value; break;
    case Projection::pvSCALE:
        _targetDef += " +k_0=" + value; break;
    case Projection::pvELLCODE:
        if ( value.indexOf("+a") != -1)
            _targetDef += " " + value;
        else
            _targetDef += " ellps=" + value;
        break;
    case Projection::pvNORTH:
        _targetDef += value == "No" ? " +south" : ""; break;
    case Projection::pvHEIGHT:
        _targetDef += " +h=" + value; break;
    default:
        _targetDef += "";
    }
    if ( _pjBase)
        pj_free(_pjBase);

    _pjBase = pj_init_plus(_targetDef.toLatin1());
}

ProjectionImplementationProj4::ProjectionImplementationProj4(const Resource &resource) : ProjectionImplementation(resource.code())
{
    QString cd = resource.code();
    _outputIsLatLon = cd == "latlong" || cd == "longlat";
    _targetDef = QString("+proj=%1").arg(cd);
    _pjLatlon =  pj_init_plus("+proj=latlong +ellps=WGS84");
    _pjBase = pj_init_plus(_targetDef.toLatin1());
}

ProjectionImplementationProj4::ProjectionImplementationProj4(const QString &code)
{
    _outputIsLatLon = code.indexOf("latlong") != -1 || code.indexOf("longlat") != -1;
    _targetDef = code;
    _pjLatlon =  pj_init_plus("+proj=latlong +ellps=WGS84");
    _pjBase = pj_init_plus(_targetDef.toLatin1());
}


ProjectionImplementationProj4::~ProjectionImplementationProj4()
{
    pj_free(_pjLatlon);
    pj_free(_pjBase);
}

bool ProjectionImplementationProj4::prepare(const QString &parms)
{
    if ( parms != "") {
        Proj4Parameters proj4(parms);
        //std::function<void(double)> assign; // = [&](double v)  -> void {}

        std::map<QString,Projection::ProjectionParamValue>  alias = { { "lat_1",Projection::pvSTANDARDPARALLEL1}, { "lat_2",Projection::pvSTANDARDPARALLEL2},{ "lat_0",Projection::pvCENTRALPARALLEL}, { "k",Projection::pvSCALE},
                                                     { "lon_0",Projection::pvCENTRALMERIDIAN}, { "x_0",Projection::pvFALSEEASTING}, { "y_0",Projection::pvFALSENORTHING}, { "zone",Projection::pvZONE},{ "k_0",Projection::pvSCALE}};
        auto assign = [&](const QString& proj4Name)->void
        {
            QString sv = proj4[proj4Name];
            if ( sv != sUNDEF) {
                Projection::ProjectionParamValue v = alias[proj4Name];
                setParameter(v, sv.toDouble());
            }
        };
        QString ellps = proj4["ellps"];
        if ( ellps != sUNDEF) {
            _targetDef += " +ellps=" + ellps;
        }
        QString a = proj4["a"];
        if ( a != sUNDEF) {
            _targetDef += " +a=" + a;
        }
        QString b = proj4["b"];
        if ( b != sUNDEF) {
            _targetDef += " +b=" + b;
        }
        QString shifts = proj4["towgs84"];
        if ( shifts != sUNDEF) {
            _targetDef += " +towgs84=" + shifts;
        }
        QString h = proj4["h"];
        if ( h != sUNDEF){
            _targetDef += " +h=" + b;
        }
        QString name = proj4["datum"];
        if ( name != sUNDEF) {
            _targetDef += " +datum=" + name;
        }

        for(auto iter= alias.begin(); iter != alias.end(); ++iter) {
            assign(iter->first);
        }
        if ( _pjBase)
            pj_free(_pjBase);

        _pjBase = pj_init_plus(_targetDef.toLatin1());
        return true;
    }
    else if ( _coordinateSystem != 0) {
        if ( _coordinateSystem->ellipsoid().isValid()) {
            _targetDef += " " + _coordinateSystem->ellipsoid()->code();
        }
        if ( _coordinateSystem->datum() && _coordinateSystem->datum()->isValid()) {
            _targetDef += " " + _coordinateSystem->datum()->code();
        }
        if ( _pjBase)
            pj_free(_pjBase);

        _pjBase = pj_init_plus(_targetDef.toLatin1());
        return true;
    }
    return ERROR1(ERR_NO_INITIALIZED_1,"Projection");
}

QString ProjectionImplementationProj4::toProj4() const
{
    return _targetDef;
}

bool ProjectionImplementationProj4::canConvertToLatLon() const
{
    return true;
}

bool ProjectionImplementationProj4::canConvertToCoordinate() const
{
    return true;
}

ProjectionImplementation *ProjectionImplementationProj4::clone()
{
    ProjectionImplementationProj4 *prjimpl = new ProjectionImplementationProj4();
    copyTo(prjimpl);
    return prjimpl;
}

void ProjectionImplementationProj4::copyTo(ProjectionImplementation *prj)
{
    ProjectionImplementation::copyTo(prj);
    ProjectionImplementationProj4 *prjimpl = static_cast<ProjectionImplementationProj4 *>(prj);
    prjimpl->_outputIsLatLon = _outputIsLatLon;
    prjimpl->_pjBase = pj_init_plus(_targetDef.toLatin1());
    prjimpl->_pjLatlon = pj_init_plus("+proj=latlong +ellps=WGS84");
    prjimpl->_targetDef = _targetDef;
}

ProjectionImplementationProj4::ProjectionImplementationProj4()
{

}

Coordinate ProjectionImplementationProj4::latlon2coord(const LatLon &ll) const
{
    if ( _pjBase == 0 || _pjLatlon == 0) {
        int *err = pj_get_errno_ref();
        if (*err != 0){
            QString error(pj_strerrno(*err));
            error = "projection error:" + error;
            kernel()->issues()->log(error);
        }
        return Coordinate();
    }

    double x = ll.lon().radians();
    double y = ll.lat().radians();
    int err = pj_transform(_pjLatlon, _pjBase, 1, 1, &x, &y, NULL );
    if ( err != 0) {
        QString error(pj_strerrno(err));
        error = "projection error:" + error;
        kernel()->issues()->log(error);
        return Coordinate();
    }

    if ( _outputIsLatLon) {
        //return Coordinate(Degrees(y,false).degrees(),Degrees(x, false).degrees());
        return Coordinate(x * RAD_TO_DEG, y * RAD_TO_DEG);
    }
    return Coordinate(x,y);
}

LatLon ProjectionImplementationProj4::coord2latlon(const Coordinate &crd) const
{

    if ( _pjBase == 0 || _pjLatlon == 0){
        int *err = pj_get_errno_ref();
        if (err != 0){
            QString error(pj_strerrno(*err));
            error = "projection error:" + error;
            kernel()->issues()->log(error);
        }
        return LatLon();
    }

    double x = crd.x;
    double y = crd.y;
    int err = pj_transform(_pjBase,_pjLatlon, 1, 1, &x, &y, NULL );
    if ( err != 0) {
        QString error(pj_strerrno(err));
        error = "projection error:" + error;
        kernel()->issues()->log(error);
        return LatLon();
    }

    return LatLon(Angle(y,true),Angle(x, true));
}


