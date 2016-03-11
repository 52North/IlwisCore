#include <QString>
#include <functional>

#include "kernel.h"
#include "ilwis.h"
#include "geometries.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "projectionimplementation.h"
#include "proj4parameters.h"
#include "box.h"
#include "coordinatesystem.h"
#include "conventionalcoordinatesystem.h"


using namespace Ilwis;

std::map<QString, QString> Projection::_projcode2Name;


void Projection::fillProjectionCodes()
{
    if ( _projcode2Name.size() == 0){
        InternalDatabaseConnection db;
        QString query = "Select code, name from projection";
        db.exec(query);
        while(db.next()){
            QString code = db.value(0).toString();
            QString nme = db.value(1).toString();
            _projcode2Name[code] = nme;
        }
    }
}

Projection::Projection() : _wkt(sUNDEF), _authority(sUNDEF)
{
    fillProjectionCodes();
}

Projection::Projection(const Resource &res)
{
   fillProjectionCodes();
}

Projection::~Projection()
{
}

Coordinate Projection::latlon2coord(const LatLon &ll) const
{
    if ( _implementation.isNull()) {
        ERROR1(ERR_NO_INITIALIZED_1, name());

        return crdUNDEF;
    }
    return _implementation->latlon2coord(ll);
}

LatLon Projection::coord2latlon(const Coordinate &crd) const
{
    if ( !_implementation.isNull())
        return _implementation->coord2latlon(crd);
    return LatLon();

}

bool Projection::prepare(const QString &parms)
{
    if ( !_implementation.isNull())
        return _implementation->prepare(parms);
    return false;
}

// TODO: this method should be merged with the method above
bool Projection::prepare(const IOOptions& options)
{
    return IlwisObject::prepare(options);
}

void Projection::setImplementation(ProjectionImplementation *impl)
{
    _implementation.reset(impl);
}


QVariant Projection::parameter(Projection::ProjectionParamValue type) const
{
    if ( _implementation.isNull()) {
        ERROR1(ERR_NO_INITIALIZED_1, name());
        return QVariant();
    }
    return _implementation->parameter(type);
}

bool Projection::isSet(Projection::ProjectionParamValue type) const
{
    if ( _implementation.isNull()) {
        ERROR1(ERR_NO_INITIALIZED_1, name());
        return false;
    }
    return _implementation->isSet(type);
}

bool Projection::isUsed(Projection::ProjectionParamValue type) const
{
    if ( _implementation.isNull()) {
        ERROR1(ERR_NO_INITIALIZED_1, name());
        return false;
    }
    return _implementation->isUsed(type);
}

IlwisTypes Projection::valueType(Projection::ProjectionParamValue type) const
{
    if ( _implementation.isNull()) {
        ERROR1(ERR_NO_INITIALIZED_1, name());
        return itUNKNOWN;
    }
    return _implementation->valueType(type);
}

void Projection::setParameter(Projection::ProjectionParamValue type, const QVariant &value)
{
    if ( _implementation.isNull()) {
        ERROR1(ERR_NO_INITIALIZED_1, name());
        return;
    }
    _implementation->setParameter(type, value);
}

QString Projection::parameterName(Projection::ProjectionParamValue pv)
{
    switch (pv){
    case pvFALSEEASTING:
        return "false easting";
    case pvFALSENORTHING:
        return "false northing";
    case pvCENTRALPARALLEL:
        return "latitude of origin";
    case pvSTANDARDPARALLEL1:
        return "standard parallel 1";
    case pvSTANDARDPARALLEL2:
        return "standard parallel 2";
    case pvLATITUDEOFTRUESCALE:
        return "latitude of true scale";
    case pvCENTRALMERIDIAN:
        return "central meridian";
    case pvSCALE:
        return "scale factor";
    case pvZONE:
        return "zone";

     default:
        break;
    }
    return "";
}

Projection::ProjectionParamValue Projection::parameterName2type(const QString& name)
{
    if (name.indexOf("false easting") == 0)
        return pvFALSEEASTING;
    if (name.indexOf("false northing")==0)
        return pvFALSENORTHING;
    if( name.indexOf("latitude of origin")==0)
        return pvCENTRALPARALLEL;
    if( name.indexOf("standard parallel 1")==0)
        return pvSTANDARDPARALLEL1;
    if( name.indexOf("standard parallel 2")==0)
        return pvSTANDARDPARALLEL2;
    if ( name.indexOf("latitude of true scale")==0)
        return pvLATITUDEOFTRUESCALE;
    if ( name.indexOf("central meridian") == 0)
        return pvCENTRALMERIDIAN;
    if ( name.indexOf("scale factor")==0)
        return pvSCALE;
    if ( name.indexOf("zone")==0)
        return pvZONE;
    if ( name.indexOf("tilt projection plane")==0)
        return pvTILT;
    if (name.indexOf("height") == 0)
        return pvHEIGHT;
    if (name.indexOf("azimuth y axis") == 0)
        return pvAZIMYAXIS;
    if ( name.indexOf("north oriented") == 0)
        return pvNORTH;



    return pvNONE;
}

QString Projection::projectionCode2Name(const QString &code)
{
    auto iter = _projcode2Name.find(code);
    if ( iter != _projcode2Name.end())
         return iter->second;
    return sUNDEF;
}

QString Projection::authority() const
{
    return _authority;
}

void Projection::setAuthority(const QString& auth){
    _authority = auth;
}

QString Projection::wkt() const
{
    return _wkt;
}

void Projection::setWkt(const QString &wkt)
{
    _wkt = wkt;
}

QString Projection::toProj4() const
{
    if ( _implementation.isNull()) {
        ERROR1(ERR_NO_INITIALIZED_1, name());
        return sUNDEF;
    }
    return _implementation->toProj4();
}

IlwisTypes Projection::ilwisType() const
{
    return itPROJECTION;
}

QString Projection::toWKT(quint32 spaces)
{
  if ( _implementation.isNull()) {
      return sUNDEF;
  }
  QString proj = "PROJECTION[\"" + _wkt + "\"],";
  proj += _implementation->toWKT(spaces);
  return proj;
}

void Projection::setCoordinateSystem(ConventionalCoordinateSystem *csy)
{
    if ( _implementation.isNull()) {
        ERROR1(ERR_NO_INITIALIZED_1, name());
        return;
    }
    _implementation->setCoordinateSystem(csy);
}

Projection::FindMatch Projection::find(const QString &v1, const QString &value)
{
    if ( v1 == value)
        return fmMatch;
    return fmNoValue;
}


bool Projection::isEqual(const IlwisObject *obj) const
{
    if ( !obj || !hasType(obj->ilwisType(), itPROJECTION))
        return false;

    if(id() == obj->id())
        return true;

    const Projection *prj = static_cast<const Projection *>(obj);
    if ( !_implementation.isNull()) {
        return _implementation->isEqual(prj->_implementation)    ;
    }
    return false;
}

bool Projection::isValid() const
{
    return !_implementation.isNull();
}

bool Projection::canConvertToLatLon() const
{
    if ( isValid())    {
        return _implementation->canConvertToLatLon();
    }
    return false;
}

bool Projection::canConvertToCoordinate() const
{
    if ( isValid())    {
        return _implementation->canConvertToCoordinate();
    }
    return false;
}

QStringList Projection::parameterNameList() const{
    InternalDatabaseConnection db("Select parameters from projection where code='" + code() + "'");
    if (!db.next()){
        return QStringList();
    }
    QString parameters = db.value(0).toString();
    QStringList parmlist = parameters.split("|");
    QStringList cleanedParms;
    for(auto parm : parmlist){
        cleanedParms.push_back(parm.split("=")[0].trimmed());
    }

    return cleanedParms;
}

IlwisObject *Projection::clone()
{
    Projection *prj = new Projection();
    copyTo(prj);
    return prj;
}

void Projection::copyTo(IlwisObject *obj)
{
    Locker<> lock(_mutex);
    IlwisObject::copyTo(obj);
    Projection *prj = static_cast<Projection *>(obj);
    prj->_wkt = _wkt;
    prj->_authority = _authority;
    prj->setImplementation(_implementation->clone());
}





