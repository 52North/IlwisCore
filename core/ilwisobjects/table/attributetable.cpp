#include "kernel.h"
#include "ilwis.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "valuedefiner.h"
#include "columndefinition.h"
#include "connectorinterface.h"
#include "table.h"
#include "attributetable.h"

using namespace Ilwis;

AttributeTable::AttributeTable()
{
}

quint32 AttributeTable::rows() const
{
    if (!_tableImpl.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return iUNDEF;
    }
    return _tableImpl->rows();
}

quint32 AttributeTable::columns() const
{
    if (!_tableImpl.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return iUNDEF;
    }
    return _tableImpl->columns();
}

QVariantList AttributeTable::record(quint32 n) const
{
    if (!_tableImpl.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return QVariantList();
    }
    return _tableImpl->record(n);
}

void AttributeTable::record(quint32 n, const QVariantList &vars, quint32 offset)
{
    if (!_tableImpl.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return ;
    }
    return _tableImpl->record(n, vars, offset);
}

QVariantList AttributeTable::column(const QString &nme) const
{
    if (!_tableImpl.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return QVariantList();
    }
    return _tableImpl->column(nme);
}

void AttributeTable::column(const QString &nme, const QVariantList &vars, quint32 offset)
{
    if (!_tableImpl.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return ;
    }
    return _tableImpl->column(nme, vars, offset);
}

QVariant AttributeTable::cell(const QString &col, quint32 rec) const
{
    if (!_tableImpl.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return QVariant();
    }
    return _tableImpl->cell(col, rec);
}

void AttributeTable::cell(const QString &col, quint32 rec, const QVariant &var)
{
    if (!_tableImpl.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return ;
    }
    return _tableImpl->cell(col, rec, var);
}

bool AttributeTable::createTable()
{
    return false;
}


ColumnDefinition AttributeTable::columndefinition(const QString &nme) const
{
    if (!_tableImpl.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return ColumnDefinition();
    }
    return _tableImpl->columndefinition(nme);
}

quint32 AttributeTable::columnIndex(const QString &nme) const
{

    if (!_tableImpl.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return iUNDEF;
    }
    return _tableImpl->columnIndex(nme);
}


bool AttributeTable::addColumn(const ColumnDefinition &def)
{
    if (!_tableImpl.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return false;
    }
    return _tableImpl->addColumn(def);
}


bool AttributeTable::addColumn(const QString &nme, const IDomain &domain)
{
    if (!_tableImpl.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return false;
    }
    return _tableImpl->addColumn(nme, domain);
}

QVariant AttributeTable::cellByKey(quint64 itemId, const QString& col) {
    if ( _keyColumn == sUNDEF) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
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

void AttributeTable::indexKey(){
    quint32 rec = 0;
    QVariantList values = column(_keyColumn);
    for(const QVariant& val : values) {
            _index[val.toInt()] = rec++;
    }
}

void AttributeTable::setTable(const ITable &tbl, const QString& keyColumn)
{
    _tableImpl = tbl;
    _keyColumn = keyColumn;
    _index.clear();
}
