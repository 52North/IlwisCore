#include <QString>
#include <QSharedPointer>
#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "domain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "numericrange.h"
#include "itemrange.h"
#include "numericitem.h"
#include "numericitemrange.h"

using namespace Ilwis;

NumericItem::NumericItem()
{
}

NumericItem::NumericItem(const QString label, const NumericRange &vr) : Identity(label), _valuerange(vr)
{
}

bool NumericItem::isValid() const
{
    return _valuerange.isValid();
}

QString NumericItem::name() const
{
    return Identity::name();
}

DomainItem *NumericItem::clone() const
{
    NumericItem *item =  new NumericItem(name(), _valuerange);
    item->setCode(code());
    item->setDescription(description());
    return item;
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

void NumericItem::clear()
{
    _valuerange.clear();
}

Ilwis::NumericItemRange *NumericItem::createRange(){
    return new NumericItemRange();
}




