#include "kernel.h"
#include "record.h"

using namespace Ilwis;

Record::Record()
{
}

Record::Record(const std::vector<QVariant>& data, quint32 offset)
{
    if ( offset == 0){
        _data = std::vector<QVariant>(data.begin(), data.end());
        return;
    }

    quint32 col = offset;
    int cols = std::min((quint32)data.size() - offset, columnCount());
    for(const QVariant& var : data) {
        if ( col < cols){
            cell(col, var);
            ++col;
        }
    }
}

Record::Record(const Record &data)
{
    _data = std::vector<QVariant>(data.cbegin(), data.cend());
    _changed = data._changed;
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

void Record::loadData(const std::vector<IlwisTypes> &types, QDataStream &stream, const IOOptions &options)
{
    quint8 valu8;
    qint8 val8;
    quint32 valu32;
    qint32 val32;
    quint64 valu64;
    qint64 val64;
    double vald;
    float valf;
    QString vals;

    _data.resize(types.size());
    for(int col = 0; col < types.size(); ++col){
        switch(types[col]){
        case itUINT8:
            stream >> valu8;
            _data[col] = valu8;break;
        case itINT8:
            stream >> val8;
            _data[col] = val8;break;
        case itUINT32:
        case itINDEXEDITEM:
        case itTHEMATICITEM:
        case itNAMEDITEM:
        case itNUMERICITEM:
        case itPALETTECOLOR:
        case itCONTINUOUSCOLOR:
            stream >> valu32;
            _data[col] = valu32;break;
        case itINT32:
            stream >> val32;
            _data[col] = val32;break;
        case itUINT64:
            stream >> valu64;
            _data[col] = valu64;break;
        case itINT64:
            stream >> val64;
            _data[col] = val64;break;
        case itDOUBLE:
            stream >> vald;
            _data[col] = vald;break;
        case itFLOAT:
            stream >> valf;
            _data[col] = valf;break;
        case itSTRING:
            stream >> vals;
            _data[col] = vals;break;
        }
    }
}

Ilwis::Record::operator std::vector<QVariant>() const
{
    std::vector<QVariant> outdata(_data.begin(), _data.end());

    return outdata;
}

void Record::storeData(const std::vector<IlwisTypes>& types, QDataStream& stream,const IOOptions &options){
    for(int col = 0; col < _data.size(); ++col){

        switch (types[col]){
        case itUINT8:
            stream << _data[col].toUInt();break;
        case itINT8:
            stream << _data[col].toInt();break;
        case itUINT32:
        case itINDEXEDITEM:
        case itTHEMATICITEM:
        case itNAMEDITEM:
        case itNUMERICITEM:
        case itPALETTECOLOR:
        case itCONTINUOUSCOLOR:
            stream << _data[col].toUInt();break;
        case itINT32:
            stream << _data[col].toInt();break;
        case itUINT64:
            stream << _data[col].toULongLong();break;
        case itINT64:
            stream << _data[col].toLongLong();break;
        case itDOUBLE:
            stream << _data[col].toDouble();break;
        case itFLOAT:
            stream << _data[col].toFloat();break;
        case itSTRING:
            stream << _data[col].toString();break;
        }
    }
}


