#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "connectorinterface.h"
#include "itemdomain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "basetable.h"
#include "datadefinition.h"


using namespace Ilwis;

AttributeDefinition::AttributeDefinition()
{
}

bool AttributeDefinition::addColumn(const ColumnDefinition& def){

    if ( _columnDefinitionsByName.find(def.name()) != _columnDefinitionsByName.end()) { // no duplicates
        return false;
    }
    ColumnDefinition coldef = def;
    coldef.columnindex(_columnDefinitionsByIndex.size());
    _columnDefinitionsByName[coldef.name()] = _columnDefinitionsByIndex.size();
    _columnDefinitionsByIndex.push_back(coldef);

    return true;
}

bool AttributeDefinition::addColumn(const QString &name, const QString &domainname, bool readonly)
{
    IDomain dom;
    if(!dom.prepare(domainname))
        return false;
    return addColumn({name, dom, readonly});
}


bool AttributeDefinition::addColumn(const QString &name, const IDomain &domain, bool readonly)
{
    return addColumn({name, domain, readonly});
}

ColumnDefinition AttributeDefinition::columndefinition(const QString &nme) const
{
    auto iter = _columnDefinitionsByName.find(nme);
    if ( iter != _columnDefinitionsByName.end()) {
       ColumnDefinition coldef = _columnDefinitionsByIndex[(*iter).second];
       return coldef;
    }
    return ColumnDefinition();
}

ColumnDefinition AttributeDefinition::columndefinition(quint32 index) const
{
    if ( index < _columnDefinitionsByIndex.size())
        return _columnDefinitionsByIndex[index];
    return ColumnDefinition();
}

ColumnDefinition &AttributeDefinition::columndefinitionRef(quint32 index)
{
    if ( index < _columnDefinitionsByIndex.size())
        return _columnDefinitionsByIndex[index];
    throw ErrorObject(TR(QString("Invalid column index for %1 used").arg(index)));
}

const ColumnDefinition &AttributeDefinition::columndefinitionRef(quint32 index) const
{
    if ( index < _columnDefinitionsByIndex.size())
        return _columnDefinitionsByIndex[index];
    throw ErrorObject(TR(QString("Invalid column index for %1 used").arg(index)));
}

ColumnDefinition &AttributeDefinition::columndefinitionRef(const QString &columnname)
{
    auto iter = _columnDefinitionsByName.find(columnname);
    if ( iter != _columnDefinitionsByName.end()) {
       ColumnDefinition& coldef = _columnDefinitionsByIndex[(*iter).second];
       return coldef;
    }
    throw ErrorObject(TR(QString("Invalid column name %1 used").arg(columnname)));
}

void AttributeDefinition::columndefinition(const ColumnDefinition &coldef)
{
    if ( coldef.id() >=  _columnDefinitionsByIndex.size())     {
        addColumn({coldef.name(), coldef.datadef().domain<>()});
    } else {
        auto iter1 = _columnDefinitionsByName.find(coldef.name());
        if ( iter1 != _columnDefinitionsByName.end()) {
            ColumnDefinition cdef = coldef;
            cdef.columnindex((*iter1).second);
            _columnDefinitionsByIndex[cdef.columnindex()] = cdef;
            (*iter1).second = cdef.columnindex();

        }else {
            addColumn(coldef);
        }
    }
}

quint32 AttributeDefinition::columnIndex(const QString &nme) const
{
    auto iter = _columnDefinitionsByName.find(nme);
    if ( iter == _columnDefinitionsByName.end()) {
        //kernel()->issues()->log(TR(ERR_COLUMN_MISSING_2).arg(nme).arg(name()),IssueObject::itWarning);
        return iUNDEF;
    }
    return (*iter).second;
}

ColumnDefinition AttributeDefinition::operator[](quint32 index)
{
    return columndefinition(index);
}

std::vector<IlwisTypes> AttributeDefinition::ilwisColumnTypes() const
{
    std::vector<IlwisTypes> types(_columnDefinitionsByIndex.size());
    for(int col = 0; col < _columnDefinitionsByIndex.size(); ++col){
        types[col] = _columnDefinitionsByIndex[col].datadef().domain()->valueType();
    }
    return types;
}

void AttributeDefinition::clearAttributeDefinitions()
{
    _columnDefinitionsByIndex.clear();
    _columnDefinitionsByName.clear();
}

QVariant AttributeDefinition::checkInput(const QVariant& inputVar, quint32 columnIndex)  const{
    QVariant actualval= inputVar;
    const ColumnDefinition& coldef = columndefinitionRef(columnIndex);
    QString typenm = inputVar.typeName();
    if ( !coldef.datadef().domain<>().isValid()){
        ERROR2(ERR_NO_INITIALIZED_2,"Domain",coldef.name());
        return QVariant();
    }
    IlwisTypes domtype = coldef.datadef().domain<>()->ilwisType();
    IlwisTypes valueType = coldef.datadef().domain<>()->valueType();
    IDomain dm = coldef.datadef().domain<>();
    if ( domtype == itITEMDOMAIN){
        if ( inputVar == sUNDEF){
            return QVariant((int)iUNDEF);
        } else if ( typenm == "QString"){
            actualval = dm->impliedValue(inputVar);
            SPItemRange rng1 = dm->range<ItemRange>();
            SPItemRange rng2 = coldef.datadef().range<ItemRange>();

            SPDomainItem item = rng1->item(inputVar.toString());
            if ( item.isNull()){
                return QVariant((int)iUNDEF);
            }
            if ( !rng2->contains(item->name())){
                rng2->add(item->clone());
            }
            actualval = item->raw();
        }

    }else if ( domtype == itNUMERICDOMAIN){
        if (typenm == "QString")
            actualval =  dm->impliedValue(inputVar);
        if ( hasType(valueType,itDATETIME) && actualval.value<Ilwis::Time>() == tUNDEF)
            return actualval;
        else if (isNumericalUndef(actualval.toDouble()))
            return rUNDEF;
        if (! dm->contains(actualval))
            actualval = rUNDEF;
        else {
            SPNumericRange rng = coldef.datadef().range<NumericRange>();
            if ( !rng.isNull()){
                if ( hasType(valueType,itDATETIME))
                    rng->add(actualval.value<Ilwis::Time>());
                else
                    rng->add(actualval.toDouble());
            }
        }
    } else if ( domtype == itTEXTDOMAIN){
        return dm->impliedValue(inputVar);
    }
    return actualval;
}

quint32 AttributeDefinition::definitionCount() const
{
    return _columnDefinitionsByIndex.size();
}

bool AttributeDefinition::isValid() const
{
    return _columnDefinitionsByIndex.size() != 0;
}


//-------------------------------------------------------------------------------------------
void SubFeatureDefinition::setSubDefinition(const IDomain &dom, const std::vector<QString> &items)
{
    _index2subFeature.clear();
    for( auto item : items){
        if ( !dom->contains(item)) {
            continue;
        }
        _index2subFeature.push_back(item);
    }

    _subFeatureDomain = dom;
    for(auto item : _index2subFeature){
        _subFeature2Index[item] = _subFeature2Index.size() - 1;
    }
}

void SubFeatureDefinition::setSubDefinition(const IDomain &dom, const std::vector<double> &items)
{
    _index2subFeature.clear();
    _subFeatureDomain = dom;
    for( auto item : items){
        if ( !dom->contains(item)) {
            continue;
        }

        if ( dom->valueType() == itINTEGER){
            _index2subFeature.push_back(QString::number((qint64)item));
        }else {
            _index2subFeature.push_back(QString::number(item));
        }
    }
    for(auto item : _index2subFeature){
        _subFeature2Index[item] = _subFeature2Index.size() - 1;
    }
}

void SubFeatureDefinition::setSubDefinition(quint32 number)
{
    _index2subFeature.clear();
    _subFeatureDomain = IDomain("count");
    for(quint32 i = 0; i < number; ++i){
        QString key = QString::number(i);
        _index2subFeature.push_back(key);
        _subFeature2Index[key] = i;
    }
}

quint32 SubFeatureDefinition::index(const QString &domainItem) const
{
    if ( !_subFeatureDomain.isValid())
        return iUNDEF;

    auto iter = _subFeature2Index.find(domainItem) ;
    if ( iter != _subFeature2Index.end())
        return iter->second;
    return iUNDEF;

}

quint32 SubFeatureDefinition::index(double domainItem) const
{
    if ( !_subFeatureDomain.isValid())
        return iUNDEF;

    double value = _subFeatureDomain->valueType() == itINTEGER ? (qint64)domainItem : domainItem;
    auto iter = _subFeature2Index.find(QString::number(value)) ;
    if ( iter != _subFeature2Index.end())
        return iter->second;
    return iUNDEF;
}

QString SubFeatureDefinition::index(quint32 idx) const
{
    if ( idx < _index2subFeature.size())
        return _index2subFeature[idx];
    return sUNDEF;
}

void SubFeatureDefinition::insert(const QString& domainItem)
{
    if (!_subFeatureDomain.isValid()) {
        kernel()->issues()->log(TR("No valid stack domain; Please set the stack domain before adding bands!"),IssueObject::itWarning);
        return;
    }
    if (!_subFeatureDomain->contains(domainItem))
        return;
    auto iter = _subFeature2Index.find(domainItem) ;
    if ( iter == _subFeature2Index.end()) { // insert one item
        _index2subFeature.push_back(domainItem);
        _subFeature2Index[domainItem] = _subFeature2Index.size() - 1;
    }
}

void SubFeatureDefinition::insert(double domainItem)
{
    if (!_subFeatureDomain.isValid()) {
        _index2subFeature.clear();
        _subFeatureDomain = IDomain("count");
        _subFeature2Index.clear();
    }
    if (!_subFeatureDomain->contains(domainItem))
        return;
    double value = _subFeatureDomain->valueType() == itINTEGER ? (qint64)domainItem : domainItem;
    auto iter = _subFeature2Index.find(QString::number(value));
    if ( iter == _subFeature2Index.end()) { // insert one item
        if ( _subFeatureDomain->valueType() == itINTEGER){
            _index2subFeature.push_back(QString::number((qint64)domainItem));
            _subFeature2Index[QString::number((qint64)domainItem)] = _subFeature2Index.size() - 1;
        }else {
            _index2subFeature.push_back(QString::number(domainItem));
            _subFeature2Index[QString::number(domainItem)] = _subFeature2Index.size() - 1;
        }
    }
}

std::vector<QString> SubFeatureDefinition::indexes() const
{
    return _index2subFeature;
}

quint32 SubFeatureDefinition::count() const
{
    return _index2subFeature.size();
}

IDomain SubFeatureDefinition::domain() const
{
    return _subFeatureDomain;
}

void SubFeatureDefinition::clearSubFeatureDefinitions()
{
    _index2subFeature.clear();
    _subFeature2Index.clear();
}

//------------------------------------------------------------------------------
void FeatureAttributeDefinition::featureAttributeDefinition(FeatureAttributeDefinition *fad)
{
    _subFeatureDefinition.reset(fad);
    _dummy.reset(new FeatureAttributeDefinition());
}

FeatureAttributeDefinition &FeatureAttributeDefinition::featureAttributeDefinitionRef(int level)
{
    if(!_subFeatureDefinition.get())
        return *(_dummy.get());
    if ( level <= 0)
        return *(_subFeatureDefinition.get());
    return _subFeatureDefinition->featureAttributeDefinitionRef(level - 1);
}

const FeatureAttributeDefinition& FeatureAttributeDefinition::featureAttributeDefinition(int level) const
{
    if ( level <= 0)
        return *(_subFeatureDefinition.get());
    return _subFeatureDefinition->featureAttributeDefinition(level - 1);
}

void FeatureAttributeDefinition::copyTo(Ilwis::FeatureAttributeDefinition &def) const
{
    def._dummy.reset(new FeatureAttributeDefinition());
    def._columnDefinitionsByIndex = _columnDefinitionsByIndex;
    def._columnDefinitionsByName = _columnDefinitionsByName;
    def._subFeatureDomain = _subFeatureDomain;
    def._index2subFeature = _index2subFeature;
    def._subFeature2Index = _subFeature2Index;
    if ( _subFeatureDefinition)
        def._subFeatureDefinition.reset(new FeatureAttributeDefinition());
}




