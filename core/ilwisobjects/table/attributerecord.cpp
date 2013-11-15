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
            return _coverageTable->columnCount();
    }
    else if ( _indexTable.isValid())
        return _indexTable->columnCount();
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


QVariant AttributeRecord::cellByKey(quint64 key, quint32 colIndex, int index) {
    if ( _keyColumn == sUNDEF) {
        ERROR1(ERR_NO_INITIALIZED_1,"key column");
        return QVariant();
    }
    if ( index == -1) {
        if ( _coverageIndex.size() == 0) {
            indexKeyColumn();
        }
        auto iter = _coverageIndex.find(key);
        if ( iter == _coverageIndex.end()) {
            return QVariant();
        }
        return _coverageTable->cell(colIndex,(*iter).second);
    } else {
        if ( _verticalIndex[index].size() == 0) {
            indexVerticalIndex(index);
        }
        auto iter = _coverageIndex.find(key);
        if ( iter == _coverageIndex.end()) {
            return QVariant();
        }
        return _indexTable->cell(colIndex,(*iter).second);
    }
    return QVariant();
}

void AttributeRecord::cellByKey(quint64 key, quint32 colIndex, const QVariant& var, int index) {
    if ( _keyColumn == sUNDEF) {
        ERROR1(ERR_NO_INITIALIZED_1,"key column");
        return;
    }
    if ( index == COVERAGETABLE) {
        if ( _coverageIndex.size() == 0) {
            indexKeyColumn();
        }
        quint32 recIndex = key;
        if ( key != NEW_RECORD) {
            auto iter = _coverageIndex.find(key);
            if ( iter == _coverageIndex.end())
                return;
            else
                recIndex= (*iter).second;
        }

        return _coverageTable->cell(colIndex,recIndex, var);
    } else {
        if ( _verticalIndex[index].size() == 0) {
            indexVerticalIndex(index);
        }
        auto iter = _coverageIndex.find(key);
        if ( iter == _coverageIndex.end()) {
            return;
        }
        _indexTable->cell(colIndex,(*iter).second, var);
    }
}

QVariant AttributeRecord::cellByRecord(quint64 record, quint32 colIndex, int index) const
{
    if ( index == COVERAGETABLE) {
        return _coverageTable->cell(colIndex, record);
    } else {
        return _indexTable->cell(colIndex, record);
    }
    return QVariant();
}

QVariant AttributeRecord::cellByRecord(quint64 record, quint32 colIndex, const QVariant& var, int index)
{
    if ( index == COVERAGETABLE) {
        _coverageTable->cell(colIndex, record,var);
    } else {
        _indexTable->cell(colIndex, record, var);
    }
    return QVariant();
}

void AttributeRecord::indexVerticalIndex(int index){
    quint32 rec = 0;
    std::vector<QVariant> values = _indexTable->column(_keyColumn);
    for(const QVariant& val : values) {
        _verticalIndex[index][val.toInt()] = rec++;
    }

}

void AttributeRecord::indexKeyColumn(){
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
