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
#include "identifieritem.h"
#include "thematicitem.h"
#include "numericitem.h"
#include "numericitemrange.h"

using namespace Ilwis;

NumericItem::NumericItem()
{
}

NumericItem::NumericItem(const QString label, const NumericRange &vr) : ThematicItem({label}), _valuerange(vr)
{
}

bool NumericItem::isValid() const
{
    return _valuerange.isValid();
}

DomainItem *NumericItem::clone() const
{
    NumericItem *item =  new NumericItem(name(), _valuerange);
    item->code(code());
    item->description(description());
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




