#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "connectorinterface.h"
#include "table.h"
#include "attributerecord.h"

using namespace Ilwis;

AttributeRecord::AttributeRecord()
{
}

AttributeRecord::AttributeRecord(const ITable &attTable, const QString &keyColumn) : _coverageTable(attTable), _keyColumn(keyColumn)
{
}

quint32 AttributeRecord::columns(bool coverages) const
{
    if ( coverages) {
        if ( _coverageTable.isValid())
            return _coverageTable->columns();
    }
    else if ( _indexTable.isValid())
        return _indexTable->columns();
    return iUNDEF;
}


ColumnDefinition AttributeRecord::columndefinition(const QString &nme, bool coverages) const
{
    if ( coverages ) {
        if ( _coverageTable.isValid())
            return _coverageTable->columndefinition(nme);
    } else if ( _indexTable.isValid() )
        return _indexTable->columndefinition(nme);

    return ColumnDefinition();
}

quint32 AttributeRecord::columnIndex(const QString &nme, bool coverages) const
{
    if ( coverages) {
        if (_coverageTable.isValid()) {
            return _coverageTable->columnIndex(nme);
        }
    } else if ( _indexTable.isValid() ) {
        return _indexTable->columnIndex(nme)        ;
    }
    return iUNDEF;
}


bool AttributeRecord::addColumn(const ColumnDefinition &def, bool coverages)
{
    if ( coverages ) {
        if (_coverageTable.isValid()) {
        return _coverageTable->addColumn(def);
        }
    } else if ( _indexTable.isValid() ) {
        return _indexTable->addColumn(def)        ;
    }
    return false;

}


bool AttributeRecord::addColumn(const QString &nme, const IDomain &domain, bool coverages)
{
    if ( coverages ) {
        if (_coverageTable.isValid()) {
        return _coverageTable->addColumn(nme, domain);
        }
    } else if ( _indexTable.isValid() ) {
        return _indexTable->addColumn(nme, domain) ;
    }
    return false;
}

QVariant AttributeRecord::cellByKey(quint64 key, const QString& col, int index) {
    if ( _keyColumn == sUNDEF) {
        ERROR1(ERR_NO_INITIALIZED_1,"key column");
        return QVariant();
    }
    if ( index == -1) {
        if ( _coverageIndex.size() == 0) {
            indexCOVERAGEKEYCOLUMN();
        }
        auto iter = _coverageIndex.find(key);
        if ( iter == _coverageIndex.end()) {
            return QVariant();
        }
        return _coverageTable->cell(col,(*iter).second);
    } else {
        if ( _verticalIndex[index].size() == 0) {
            indexVerticalIndex(index);
        }
        auto iter = _coverageIndex.find(key);
        if ( iter == _coverageIndex.end()) {
            return QVariant();
        }
        return _indexTable->cell(col,(*iter).second);
    }
    return QVariant();
}

QVariant AttributeRecord::cellByIndex(quint64 index, quint32 colIndex, int )
{
    if ( index < _indexTable->rows())
        return _indexTable->cell(index, colIndex);
    return QVariant();
}

void AttributeRecord::indexVerticalIndex(int index){
    quint32 rec = 0;
    std::vector<QVariant> values = _indexTable->column(_keyColumn);
    for(const QVariant& val : values) {
        _verticalIndex[index][val.toInt()] = rec++;
    }

}

void AttributeRecord::indexCOVERAGEKEYCOLUMN(){
    quint32 rec = 0;
    std::vector<QVariant> values = _coverageTable->column(_keyColumn);
    for(const QVariant& val : values) {
        _coverageIndex[val.toInt()] = rec++;
    }
}

void AttributeRecord::setTable(const ITable &tbl, const QString& keyColumn, int indexCount)
{
    if ( indexCount == -1) {
        _coverageTable = tbl;
        _keyColumn = keyColumn;
        _coverageIndex.clear();
    } else {
        _verticalIndex.resize(indexCount);
        _indexTable = tbl;
    }
}

bool AttributeRecord::isValid() const
{
    return _coverageTable.isValid() && _keyColumn != "";
}
