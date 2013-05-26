#include <QString>
#include "module.h"
#include "kernel.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "numericdomain.h"
#include "range.h"
#include "numericrange.h"
#include "itemrange.h"
#include "datadefinition.h"

using namespace Ilwis;

DataDefinition::DataDefinition() : _range(0)
{
}


DataDefinition::DataDefinition(const DataDefinition& def)
{
    domain(def.domain());
    if ( !def.range().isNull())
        _range.reset(def.range()->clone());
    else
        _range.reset(0);
}

DataDefinition::DataDefinition(const IDomain &dm, Range *rng)
{
    domain(dm);
    if ( rng)
        _range.reset(rng);
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
    if ( _domain.isValid() && _domain->ilwisType() == itNUMERICDOMAIN) {
        INumericDomain nd = _domain.get<NumericDomain>();
        Range *r = nd->range<Range>()->clone();
        if ( r)
            _range.reset(r);
    }
}

bool DataDefinition::isValid() const
{
    return _domain.isValid();
}


DataDefinition operator +(const DataDefinition &def1, const DataDefinition &def2)
{
    IDomain dm;
    if ( !def1.isValid() && def2.isValid())
        return def2;
    if ( !def2.isValid() && def1.isValid())
        return def1;

    if ( def1.domain()->ilwisType() == itNUMERICDOMAIN && def1.domain()->ilwisType() == itNUMERICDOMAIN) {
        if ( def1.domain()->name() == "boolean" && def1.domain()->name() == "boolean") {
            return DataDefinition(def1.domain());
        }
        SPNumericRange nr1 = def1.range().dynamicCast<NumericRange>();
        SPNumericRange nr2 = def1.range().dynamicCast<NumericRange>();
        NumericRange *nrNew = NumericRange::merge(nr1, nr2);
        if ( def1.domain()->name() == def1.domain()->name()) {
            return DataDefinition(def1.domain(), nrNew);
        } else {
            dm.prepare("value");
            return DataDefinition(dm, nrNew);
        }
    }
    if ( def1.domain()->ilwisType() == itITEMDOMAIN && def1.domain()->ilwisType() == itITEMDOMAIN) {
        SPItemRange nr1 = def1.range().dynamicCast<ItemRange>();
        SPItemRange nr2 = def1.range().dynamicCast<ItemRange>();
        ItemRange *nrNew = ItemRange::merge(nr1, nr2);
        if ( def1.domain()->name() == def1.domain()->name()) {
            return DataDefinition(def1.domain(), nrNew);
        } else {
            dm.prepare("value");
            return DataDefinition(dm, nrNew);
        }
    }
    return DataDefinition();

}



