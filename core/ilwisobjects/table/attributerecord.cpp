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

AttributeRecord::AttributeRecord(quint32 keyrecord, const ITable &attTable) : _coverageTable(attTable), _keyRecord(keyrecord)
{
}

quint32 AttributeRecord::columnCount(bool coverages) const
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

ColumnDefinition AttributeRecord::columndefinition(int colindex, bool coverages) const
{
    if ( coverages ) {
        if ( _coverageTable.isValid())
            return _coverageTable->columndefinition(colindex);
    } else if ( _indexTable.isValid() )
        return _indexTable->columndefinition(colindex);

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

void AttributeRecord::cell(quint32 colIndex, const QVariant &var, int index)
{
    if ( index == -1) {
        _coverageTable->setCell(colIndex, _keyRecord, var);
    } else {
        _indexTable->setCell(colIndex,_keyRecord * index, var);
    }
}


QVariant AttributeRecord::cell(quint32 colIndex, int index, bool asRaw){
    if ( index == -1) {
        return _coverageTable->cell(colIndex, _keyRecord, asRaw);
    } else {
        return _indexTable->cell(colIndex,_keyRecord * index, asRaw);
    }
    return QVariant();
}

AttributeRecord *AttributeRecord::clone() const
{
    return new AttributeRecord(_keyRecord, _coverageTable);
}

bool AttributeRecord::isValid() const
{
    return _coverageTable.isValid() && _keyRecord != iUNDEF;
}
