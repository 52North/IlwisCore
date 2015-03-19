#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "table.h"
#include "attributemodel.h"
#include "ilwistypes.h"


AttributeModel::AttributeModel()
{

}

AttributeModel::~AttributeModel()
{
}

AttributeModel::AttributeModel(const Ilwis::ColumnDefinition& def, QObject *parent, const Ilwis::IIlwisObject& obj) :
    QObject(parent),
    _coldefinition(def),
    _owner(obj)
{
    if ( def.isValid() && def.datadef().domain()->ilwisType() == itITEMDOMAIN && def.datadef().domain()->valueType() != itINDEXEDITEM){
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

QString AttributeModel::actualRangeDefintion(bool calc) const
{
    QString rangeString;
    if ( _coldefinition.isValid()){
        if ( calc || _coldefinition.datadef().range().isNull()){
            if ( hasType( _owner->ilwisType() , itFEATURE)){
                Ilwis::IFeatureCoverage features = _owner.as<Ilwis::FeatureCoverage>();
                if ( !features.isValid())
                    return "";
                features->loadData();
                std::vector<QVariant> data = features->attributeTable()->column(_coldefinition.name());
                if ( data.size() != 0){
                    std::vector<double> values(data.size());
                    int  i=0;
                    for(auto v : data)
                        values[i++] = v.toDouble();
                    Ilwis::NumericStatistics stats; // realy like to do this with a template specialization of the proper calculate function, but the syntax was unclear to me
                    stats.calculate(values.begin(), values.end());
                    Ilwis::NumericRange *rng = new Ilwis::NumericRange(stats.prop(Ilwis::NumericStatistics::pMIN),stats.prop(Ilwis::NumericStatistics::pMAX));
                    features->attributeDefinitionsRef().columndefinitionRef(_coldefinition.name()).datadef().range(rng);
                    const_cast<AttributeModel *>(this)->_coldefinition = features->attributeDefinitions().columndefinition(_coldefinition.name());
                }
            }
        }
        rangeString = _coldefinition.datadef().range()->toString();

    }
    return rangeString;
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

QString AttributeModel::attributeDomainType() const
{
    quint64 tp = _coldefinition.datadef().domain()->ilwisType();
    return Ilwis::IlwisObject::type2Name(tp);
}

QStringList AttributeModel::attributeValues(bool defaultRange) const
{
    if ( defaultRange)
        return _attributeValuesDefaultRange;
    return _attributeValuesActualRange;
}

QString AttributeModel::rangeDefinition(bool defaultRange, bool calc, const QString&)  const
{
    if ( defaultRange)
        return defaultRangeDefinition();
    return actualRangeDefintion(calc);
}

const Ilwis::ColumnDefinition &AttributeModel::columnDef() const
{
    return _coldefinition;
}

void AttributeModel::setObject(const Ilwis::IIlwisObject &obj)
{
    _owner = obj;
}


