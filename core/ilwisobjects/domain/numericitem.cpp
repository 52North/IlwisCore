#include <QString>
#include <QSharedPointer>
#include "kernel.h"
#include "range.h"
#include "numericrange.h"
#include "domainitem.h"
#include "itemrange.h"
#include "numericitem.h"
#include "numericitemrange.h"

using namespace Ilwis;

NumericItem::NumericItem()
{
}

NumericItem::NumericItem(const NumericRange &vr) : _valuerange(vr)
{
}

bool NumericItem::isValid() const
{
    return _valuerange.isValid();
}

QString NumericItem::name(quint32 index) const
{
    return _valuerange.toString();
}

DomainItem *NumericItem::clone() const
{
    return new NumericItem(_valuerange);
}

IlwisTypes NumericItem::valueTypeS()
{
    return itNUMERICITEM;
}

IlwisTypes NumericItem::valueType() const
{
    return valueTypeS();
}

NumericRange NumericItem::range() const
{
    return _valuerange;
}

void NumericItem::range(const NumericRange &valuerange)
{
    _valuerange = valuerange;
}

Ilwis::NumericItemRange *NumericItem::createRange(){
    return new NumericItemRange();
}




