#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "record.h"
#include "table.h"

using namespace Ilwis;

Record::Record()
{
}

Record::Record(const std::vector<QVariant>& data, Table *table): _table(table)
{
    _data = std::vector<QVariant>(data.begin(), data.end());
}

Record::Record(const Record &data)
{
    _table = data._table;
    _data = std::vector<QVariant>(data.cbegin(), data.cend());
}

bool Record::isChanged() const
{
    return _changed;
}

void Record::changed(bool yesno)
{
    _changed = yesno;
}

bool Record::isValid() const
{
    return _data.size() != 0;
}

ColumnDefinition Record::columnDefinition(const quint32 idx) const
{
    _table->columndefinition(idx);
}

ColumnDefinition Record::columnDefinition(const QString name) const
{
    _table->columndefinition(name);
}

quint64 Record::itemid() const
{
    return _itemid;
}

void Record::itemid(quint64 id)
{
    _itemid = id;
}

CRecordIter Record::cbegin() const noexcept
{
    return _data.begin();
}

CRecordIter Record::cend() const noexcept
{
    return _data.end();
}

QVariant Record::cell(quint32 column) const
{
    if ( column < _data.size())
        return _data.at(column);

    return QVariant();
}

void Record::cell(quint32 column, const QVariant &value)
{
    ColumnDefinition colDef = columnDefinition(column);
    if (colDef.isReadOnly()) {
        WARN1("Cannot change readonly cell of '%1'.", colDef.name());
        return;
    }

    if ( column < _data.size()){
        _data.at(column) = value;
        _changed = true;
    }
}

quint32 Record::columnCount() const
{
    return _data.size();
}

Ilwis::Record::operator std::vector<QVariant>() const
{
    std::vector<QVariant> outdata(_data.begin(), _data.end());

    return outdata;
}


