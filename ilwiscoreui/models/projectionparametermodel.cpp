#include "projectionparametermodel.h"

ProjectionParameterModel::ProjectionParameterModel()
{

}

ProjectionParameterModel::ProjectionParameterModel(const Ilwis::IProjection& proj, Ilwis::Projection::ProjectionParamValue type, QObject *parent) :
    QObject(parent)
{

   _name = proj->parameterName(type);
   if ( proj->isSet(type)){
        _value = proj->parameter(type).toString();
    }
}

ProjectionParameterModel::~ProjectionParameterModel()
{

}

QString ProjectionParameterModel::name() const
{
    return _name;
}

QString ProjectionParameterModel::value() const
{
    return _value;
}

QString ProjectionParameterModel::description() const
{
    return _description;
}

