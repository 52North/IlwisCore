#include "projectionparametermodel.h"

ProjectionParameterModel::ProjectionParameterModel()
{

}

ProjectionParameterModel::ProjectionParameterModel(const Ilwis::IProjection& proj, Ilwis::Projection::ProjectionParamValue type, QObject *parent) :
    QObject(parent)
{

   _name = Ilwis::Projection::parameterName(type);
   if ( proj->isUsed(type)){
        _value = proj->parameter(type).toString();
        _parmType = (int)type;
    }
   _valueType = proj->valueType(type);
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

QString ProjectionParameterModel::valueType() const
{
   // return "aap";
    return Ilwis::TypeHelper::type2name(_valueType);
}

int ProjectionParameterModel::parameterType() const
{
    return _parmType;
}

//bool ProjectionParameterModel::isSet(const QString& type){
//    return proj->isSet(Ilwis::Projection::parameterName2type(type));
//}

