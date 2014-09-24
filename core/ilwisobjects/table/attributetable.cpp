#include "kernel.h"
#include "coverage.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "attributedefinition.h"
#include "featurecoverage.h"
#include "feature.h"
#include "table.h"
#include "attributetable.h"

using namespace Ilwis;

AttributeTable::AttributeTable(FeatureCoverage *featureCoverage)
{
    _features.set(featureCoverage);
    setValid(_features.isValid());
}

IlwisTypes AttributeTable::ilwisType() const
{
    return itATTRIBUTETABLE;
}

quint32 AttributeTable::recordCount() const
{
    if (!_features.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return iUNDEF;
    }
    return _features->featureCount();
}

quint32 AttributeTable::columnCount() const
{
    if (!_features.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return iUNDEF;
    }
    return _features->attributeDefinitions().definitionCount();
}

Record& AttributeTable::recordRef(quint32 n)
{
    if (!_features.isValid() || n >= recordCount()) {
        throw ErrorObject(TR(QString("attribute table %1 not properly initialized").arg(name())));
    }
    return _features->_features[n]->recordRef();
}

const Record &AttributeTable::record(quint32 n) const
{
    if (!_features.isValid() || n >= recordCount()) {
        throw ErrorObject(TR(QString("attribute table %1 not properly initialized").arg(name())));
    }
    return _features->_features[n]->record();
}

void AttributeTable::record(quint32 rec, const std::vector<QVariant>& vars, quint32 offset)
{
    if (!_features.isValid() || rec >= recordCount()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return ;
    }
    _features->_features[rec]->record(vars, offset);
}

void AttributeTable::dataLoaded(bool yesno)
{
}

bool AttributeTable::isDataLoaded() const
{
    return true;
}

std::vector<QVariant> AttributeTable::column(const QString &columnName) const
{
    if (!_features.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return std::vector<QVariant>();
    }
    std::vector<QVariant> values(_features->featureCount());
    int columnIndex = _features->attributeDefinitions().columndefinition(columnName).id();
    for(int rec = 0; rec < _features->featureCount(); ++rec){
        values[rec] = _features->_features[rec]->recordRef().cell(columnIndex)   ;
    }
    return values;
}

void AttributeTable::column(const QString &columnName, const std::vector<QVariant> &vars, quint32 offset)
{
    quint32 colIndex = columnIndex(columnName);
    column(colIndex,vars, offset);
}

std::vector<QVariant> AttributeTable::column(quint32 index, quint32 start, quint32 stop) const
{
    stop = std::min(stop, recordCount());
    start = std::max((quint32)0, start);
    std::vector<QVariant> data(stop - start);
    for(quint32 i=start; i < stop; ++i) {
        data[i - start] = _features->_features[i](index);
    }
    return data;
}

void AttributeTable::column(const quint32 columnIndex, const std::vector<QVariant> &vars, quint32 offset)
{
    if (!_features.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return ;
    }
    if ( !columnIndex == iUNDEF)
        return ;

    quint32 rec = offset;
    for(const QVariant& var : vars) {
        if ( rec < recordCount()){
            _features->_features[rec++](columnIndex, var);
        }
    }
}

QVariant AttributeTable::cell(const QString &columnName, quint32 rec, bool asRaw) const
{
    quint32 index = columnIndex(columnName);
    return cell(index, rec, asRaw);
}

QVariant AttributeTable::cell(const quint32 index, quint32 rec, bool asRaw) const
{
    if (!_features.isValid() || rec >= recordCount()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return QVariant();
    }
    return _features->_features[rec]->cell(index, asRaw);
}

void AttributeTable::recordCount(quint32 r)
{
    WARN2(ERR_OPERATION_NOTSUPPORTED2,TR("adding columns"), TR("attributes"));
}

void AttributeTable::setCell(const QString &columnName, quint32 rec, const QVariant &var)
{
    quint32 index = columnIndex(columnName);
    setCell(index, rec, var);
}

void AttributeTable::setCell(quint32 col, quint32 rec, const QVariant &var)
{
    if (!_features.isValid() || rec > recordCount()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return ;
    }
    _features->_features[rec](col, var);
}

bool AttributeTable::createTable()
{
    return true;
}


ColumnDefinition AttributeTable::columndefinition(const QString &columnName) const
{
    if (!_features.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return ColumnDefinition();
    }
    return _features->attributeDefinitions().columndefinition(columnName);
}

ColumnDefinition AttributeTable::columndefinition(quint32 index) const
{
    return _features->attributeDefinitions().columndefinition(index);
}

ColumnDefinition &AttributeTable::columndefinitionRef(quint32 index)
{
   return _features->attributeDefinitionsRef().columndefinitionRef(index);
}

ColumnDefinition &AttributeTable::columndefinitionRef(const QString &columnName)
{
    return _features->attributeDefinitionsRef().columndefinitionRef(columnName);
}

quint32 AttributeTable::columnIndex(const QString &columnName) const
{

    if (!_features.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return iUNDEF;
    }
    return _features->attributeDefinitions().columnIndex(columnName);
}

std::vector<QVariant> AttributeTable::column(const QString &columnName, quint32 start, quint32 stop) const
{
    quint32 index = columnIndex(columnName);
    return column(index, start, stop);
}

std::vector<quint32> AttributeTable::select(const QString &conditions) const
{
    //TODO
}


bool AttributeTable::addColumn(const ColumnDefinition &def)
{
    return WARN2(ERR_OPERATION_NOTSUPPORTED2,TR("adding columns"), TR("attributes"));
}

bool AttributeTable::addColumn(const QString &name, const QString &domainname, const bool readonly)
{
    return WARN2(ERR_OPERATION_NOTSUPPORTED2,TR("adding columns"), TR("attributes"));
}


bool AttributeTable::addColumn(const QString &nme, const IDomain &domain,const bool readonly)
{
    return WARN2(ERR_OPERATION_NOTSUPPORTED2,TR("adding columns"), TR("attributes"));
}

void AttributeTable::columndefinition(const ColumnDefinition &)
{
    WARN2(ERR_OPERATION_NOTSUPPORTED2,TR("setting column definitions"), TR("attributes"));
}


