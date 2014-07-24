#include <QString>
#include <QVariant>
#include <map>
#include "iooptions.h"

using namespace Ilwis;

bool IOOptions::contains(const QString& option) const{
    return _values.find(option) != _values.end();
}

quint32 IOOptions::size() const
{
    return _values.size();
}


QVariant IOOptions::operator[](const QString& option) const
{
    auto iter = _values.find(option);
    if ( iter != _values.end()){
        return iter->second;
    }
    return QVariant();
}

IOOptions &IOOptions::operator<<(const IOOptions::Option &option)
{
    if (!option._value.isValid()){
        _values.erase(option._key);
    }
    else if ( option._key != "?")
        _values[option._key] = option._value;
    return *this;
}
