#include "kernel.h"
#include "record.h"

using namespace Ilwis;

Record::Record()
{
}

Record::Record(const std::vector<QVariant>& data)
{
    _data = std::vector<QVariant>(data.begin(), data.end());
}

Record::Record(const Record &data)
{
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


