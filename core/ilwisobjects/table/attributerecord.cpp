#include "kernel.h"
#include "ilwis.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "connectorinterface.h"
#include "table.h"
#include "attributerecord.h"

using namespace Ilwis;

AttributeRecord::AttributeRecord(const ITable &attTable, const QString &keyColumn) : _tableImpl(attTable), _keyColumn(keyColumn)
{
}

quint32 AttributeRecord::columns() const
{
    if (!_tableImpl.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,"attribute table");
        return iUNDEF;
    }
    return _tableImpl->columns();
}


ColumnDefinition AttributeRecord::columndefinition(const QString &nme) const
{
    if (!_tableImpl.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,"attribute table");
        return ColumnDefinition();
    }
    return _tableImpl->columndefinition(nme);
}

quint32 AttributeRecord::columnIndex(const QString &nme) const
{

    if (!_tableImpl.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,"attribute table");
        return iUNDEF;
    }
    return _tableImpl->columnIndex(nme);
}


bool AttributeRecord::addColumn(const ColumnDefinition &def)
{
    if (!_tableImpl.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,"attribute table");
        return false;
    }
    return _tableImpl->addColumn(def);
}


bool AttributeRecord::addColumn(const QString &nme, const IDomain &domain)
{
    if (!_tableImpl.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,"attribute table");
        return false;
    }
    return _tableImpl->addColumn(nme, domain);
}

QVariant AttributeRecord::cellByKey(quint64 itemId, const QString& col) {
    if ( _keyColumn == sUNDEF) {
        ERROR1(ERR_NO_INITIALIZED_1,"key column");
        return QVariant();
    }
    if ( _index.size() == 0) {
        indexKey();
    }
    auto iter = _index.find(itemId);
    if ( iter == _index.end()) {
        return QVariant();
    }
    return _tableImpl->cell(col,(*iter).second);

}

void AttributeRecord::indexKey(){
    quint32 rec = 0;
    QVariantList values = _tableImpl->column(_keyColumn);
    for(const QVariant& val : values) {
            _index[val.toInt()] = rec++;
    }
}

void AttributeRecord::setTable(const ITable &tbl, const QString& keyColumn)
{
    _tableImpl = tbl;
    _keyColumn = keyColumn;
    _index.clear();
}
