#include <functional>

#include "kernel.h"
#include "ilwis.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "coordinatesystem.h"
#include "proj4parameters.h"
#include "conventionalcoordinatesystem.h"
#include "projectionimplementation.h"

using namespace Ilwis;

ProjectionImplementation::ProjectionImplementation(const QString &type) :
    _coordinateSystem(0),
    _projtype(type)
{
    initParameterList(type);
}

QString ProjectionImplementation::type() const
{
    return _projtype;
}

void ProjectionImplementation::setCoordinateSystem(ConventionalCoordinateSystem *csy)
{
    _coordinateSystem = csy;
}

QVariant ProjectionImplementation::parameter(Projection::ProjectionParamValue type) const
{
    auto iter = _parameters.find(type);
    if ( iter != _parameters.end()) {
        return (*iter).second._value;
    }
    return QVariant();
}

void ProjectionImplementation::setParameter(Projection::ProjectionParamValue type, const QVariant &value)
{
    bool used = _parameters[type]._isUsed;
    _parameters[type] = {value, _parameters[type]._valueType,true};
   _parameters[type]._isUsed = used;
}

bool ProjectionImplementation::isEqual(const QScopedPointer<ProjectionImplementation>& projimpl) {
    QString proj4a = toProj4();
    QString proj4b = projimpl->toProj4();

    return proj4a == proj4b;
}

bool ProjectionImplementation::isSet(Projection::ProjectionParamValue type) const{
    auto iter = _parameters.find(type);
    if ( iter != _parameters.end()) {
        return (*iter).second._isSet;
    }
    return false;
}

bool ProjectionImplementation::isUsed(Projection::ProjectionParamValue type) const
{
    auto iter = _parameters.find(type);
    if ( iter != _parameters.end()) {
        return (*iter).second._isUsed;
    }
    return false;
}

IlwisTypes ProjectionImplementation::valueType(Projection::ProjectionParamValue type) const
{
    auto iter = _parameters.find(type);
    if ( iter != _parameters.end()) {
        return (*iter).second._valueType;
    }
    return itUNKNOWN;
}

void ProjectionImplementation::copyTo(ProjectionImplementation *prj)
{
    prj->_projtype = _projtype;
    prj->_parameters = _parameters;
}

void ProjectionImplementation::initParameterList(const QString &projtype)
{
    _parameters[Projection::pvFALSEEASTING] = {0};
    _parameters[Projection::pvFALSENORTHING] = {0};
    _parameters[Projection::pvSCALE] = {1};
    _parameters[Projection::pvCENTRALPARALLEL] = {0, itLATLON};
    _parameters[Projection::pvSTANDARDPARALLEL1] = {30, itLATLON};
    _parameters[Projection::pvSTANDARDPARALLEL2] = {60, itLATLON};
    _parameters[Projection::pvLATITUDEOFTRUESCALE] = {0, itLATLON};
    _parameters[Projection::pvZONE] = {1, itINT8};
    _parameters[Projection::pvNORIENTED] =  {true, itBOOL};
    _parameters[Projection::pvAZIMYAXIS] = {0};
    _parameters[Projection::pvTILTED] =  {false,itBOOL};
    _parameters[Projection::pvAZIMCLINE] = {0};
    _parameters[Projection::pvHEIGHT] = {0};
    _parameters[Projection::pvCENTRALMERIDIAN] =  {0, itLATLON};
     InternalDatabaseConnection projs(QString("select parameters from projection where code='%1'").arg(projtype));
     while(projs.next()){
        QString parms = projs.value(0).toString();
        QStringList parts = parms.split("|");
        for(const QString& part : parts){
            Projection::ProjectionParamValue tp = Projection::parameterName2type(part);
            _parameters[tp]._isUsed = true;
        }
     }
}

QString ProjectionImplementation::toWKT(quint32 spaces)
{
    std::map<Projection::ProjectionParamValue, QString>  kvp = { { Projection::pvSTANDARDPARALLEL1, "standard_parallel_1"}, { Projection::pvSTANDARDPARALLEL2,"standard_parallel_2"},{ Projection::pvCENTRALPARALLEL,"latitude_of_origin"}, { Projection::pvSCALE,"scale_factor"},
                                                                 { Projection::pvCENTRALMERIDIAN,"central_meridian"}, { Projection::pvFALSEEASTING,"false_easting"}, { Projection::pvFALSENORTHING,"false_northing"}};
    QString result;
    QString indent = QString(" ").repeated(spaces);
    QString ending = spaces == 0 ? "" : "\n";

    bool isUTM = (_coordinateSystem != 0) ? _coordinateSystem->projection()->code() == "utm" : false;
    for(auto parm : _parameters){
        if ( parm.second._isSet){
            if ( parm.first == Projection::pvZONE){
                if ( isUTM){
                    result += indent + QString("PARAMETER[\"scale_factor\",0.9996],") + ending;
                    result += indent + QString("PARAMETER[\"false_easting\",500000],") + ending;
                    result += indent + QString("PARAMETER[\"false_northing\",0],") + ending;
                    result += indent + QString("PARAMETER[\"scale\",0.9996],")  + ending;
                    result += indent + QString("PARAMETER[\"latitude_of_origin\",0],") + ending;
                    result += indent + QString("PARAMETER[\"central_meridian\",%1]").arg(parm.second._value.toDouble() * 6.0 - 183.0);
                }
            }else {
                if ( parm.first != Projection::pvELLCODE){
                    QString name =kvp[parm.first];
                    QString v = parm.second._value.toString();
                    if ( result != "")
                        result += ",";
                    result += indent + QString("PARAMETER[%1,%2]").arg(name).arg(v)  + ending ;
                }
            }
        }
    }
    return result;
}


