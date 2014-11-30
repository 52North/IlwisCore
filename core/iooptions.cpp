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

bool IOOptions::isEmpty() const
{
    return size() == 0;
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
    return addOption(option._key, option._value);
}

IOOptions &IOOptions::addOption(const QPair<QString, QVariant>& item)
{
    return addOption(item.first, item.second);
}

IOOptions &IOOptions::addOption(const QString &key, const QVariant &value)
{
    if (!value.isValid()){
        _values.erase(key);
    }
    else if ( key != "?")
        _values[key] = value;
    return *this;
}
