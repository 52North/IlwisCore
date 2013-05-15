#include <QString>
#include "module.h"
#include "kernel.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "numericdomain.h"
#include "range.h"
#include "numericrange.h"
#include "valuedefiner.h"

using namespace Ilwis;

ValueDefiner::ValueDefiner() : _range(0)
{
}

Ilwis::PRange ValueDefiner::range() const
{
    return _range;
}

void ValueDefiner::setRange(Range* vr)
{
    _range = QSharedPointer<Range>(vr);
}

IDomain ValueDefiner::domain() const
{
    return _domain;
}

void ValueDefiner::setDomain(const IDomain &dom)
{
    _domain = dom;
    if ( _domain->ilwisType() == itNUMERICDOMAIN) {
        INumericDomain nd = _domain.get<NumericDomain>();
        Range *r = nd->range<Range>()->clone();
        if ( r)
            _range.reset(r);
    }
}


