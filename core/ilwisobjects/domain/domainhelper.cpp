#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "domain.h"
#include "itemrange.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "numericdomain.h"
#include "numericrange.h"
#include "identifierrange.h"
#include "juliantime.h"
#include "domainhelper.h"

using namespace Ilwis;

DomainHelper::DomainHelper()
{
}

IDomain DomainHelper::create(Range* range) {
    IDomain domain;
    switch(range->valueType()){
    case itINTEGER:
    case itFLOAT:
    case itDOUBLE:
    case itTIME:
    case itDATE:
    case itDATETIME:
        domain.set(new NumericDomain(static_cast<NumericRange *>(range)));break;
    case itTHEMATICITEM:
        domain.set(new ThematicDomain(range)); break;
    case itNAMEDITEM:
        domain.set(new NamedIdDomain(range)); break;

    };
    return domain;
}

IDomain DomainHelper::fromTimeInterval(const QString& beg, const QString& end, const Duration &step) {
    return DomainHelper::create(new TimeInterval(beg, end, step));
}

IDomain DomainHelper::fromNumericRange(double beg, double end, double step) {
    return DomainHelper::create(new NumericRange(beg, end, step));
}

