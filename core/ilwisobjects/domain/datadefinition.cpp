#include <QString>
#include "module.h"
#include "kernel.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "numericdomain.h"
#include "range.h"
#include "numericrange.h"
#include "datadefinition.h"

using namespace Ilwis;

DataDefinition::DataDefinition() : _range(0)
{
}


DataDefinition::DataDefinition(const DataDefinition& def)
{
    domain(def.domain());
    _range.reset(def.range()->clone());
}

DataDefinition::~DataDefinition()
{
}

Ilwis::PRange DataDefinition::range() const
{
    return _range;
}

void DataDefinition::range(Range* vr)
{
    _range = QSharedPointer<Range>(vr);
}

IDomain DataDefinition::domain() const
{
    return _domain;
}

void DataDefinition::domain(const IDomain &dom)
{
    _domain = dom;
    if ( _domain->ilwisType() == itNUMERICDOMAIN) {
        INumericDomain nd = _domain.get<NumericDomain>();
        Range *r = nd->range<Range>()->clone();
        if ( r)
            _range.reset(r);
    }
}


