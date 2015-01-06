#include "attributemodel.h"
#include "ilwistypes.h"

AttributeModel::AttributeModel()
{

}

AttributeModel::AttributeModel(const Ilwis::ColumnDefinition& def, QObject *parent) :
    QObject(parent),
    _coldefinition(def)
{
    if ( def.datadef().domain()->ilwisType() == itITEMDOMAIN && def.datadef().domain()->valueType() != itINDEXEDITEM){
        if (!def.datadef().domain()->range().isNull()){
            QString defset = def.datadef().domain()->range()->toString();
            defset = defset.mid(defset.indexOf(":") + 1);
            _attributeValuesDefaultRange = defset.split("|");
        }
        if (!def.datadef().range().isNull()){
            QString defset = def.datadef().domain()->range()->toString();
            defset = defset.mid(defset.indexOf(":") + 1);
            _attributeValuesActualRange = defset.split("|");
        }
    }
}


QString AttributeModel::defaultRangeDefinition() const
{
    if ( _coldefinition.isValid()){
         if ( !_coldefinition.datadef().domain()->range().isNull())
            return _coldefinition.datadef().domain()->range()->toString();
    }
    return "";
}

QString AttributeModel::actualRangeDefintion() const
{
    if ( _coldefinition.isValid()){
        if ( !_coldefinition.datadef().range().isNull())
            return _coldefinition.datadef().range()->toString();
    }
    return "";
}

QString AttributeModel::attributename() const
{
   if ( _coldefinition.isValid()){
       return _coldefinition.name();
   }
   return "";
}

QString AttributeModel::attributeDomain() const
{
  if ( _coldefinition.isValid()){
      return _coldefinition.datadef().domain()->name();
  }
  return "";
}

QString AttributeModel::attributeValueType() const
{
    if ( _coldefinition.isValid()){

        QString typeName =  Ilwis::TypeHelper::type2HumanReadable( _coldefinition.datadef().domain()->valueType());

        return typeName == sUNDEF ? "" : typeName;
    }
    return "";
}

QStringList AttributeModel::attributeValues(bool defaultRange) const
{
    if ( defaultRange)
        return _attributeValuesDefaultRange;
    return _attributeValuesActualRange;
}

QString AttributeModel::rangeDefinition(bool defaultRange)  const
{
    if ( defaultRange)
        return defaultRangeDefinition();
    return actualRangeDefintion();
}
