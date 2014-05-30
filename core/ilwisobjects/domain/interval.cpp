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
#include "interval.h"
#include "intervalrange.h"

using namespace Ilwis;

Interval::Interval()
{
}

Interval::Interval(const QString label, const NumericRange &vr) : ThematicItem({label}), _valuerange(vr)
{
}

bool Interval::isValid() const
{
    return _valuerange.isValid();
}

bool Interval::isOrdered() const
{
    return true;
}

DomainItem *Interval::clone() const
{
    Interval *item =  new Interval(name(), _valuerange);
    item->code(code());
    item->description(description());
    return item;
}

IlwisTypes Interval::valueTypeS()
{
    return itNUMERICITEM;
}

IlwisTypes Interval::valueType() const
{
    return valueTypeS();
}

NumericRange Interval::range() const
{
    return _valuerange;
}

void Interval::range(const NumericRange &valuerange)
{
    _valuerange = valuerange;
}

void Interval::clear()
{
    _valuerange.clear();
}

Ilwis::IntervalRange *Interval::createRange(){
    return new IntervalRange();
}




