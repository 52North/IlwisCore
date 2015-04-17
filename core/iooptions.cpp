#include <QString>
#include <QVariant>
#include <map>
#include "iooptions.h"

using namespace Ilwis;

bool IOOptions::isEmpty() const
{
    return size() == 0;
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
        auto iter = find(key);
        if ( iter != end())
            erase(iter);
    }
    else if ( key != "?")
        (*this)[key] = value;
    return *this;
}
