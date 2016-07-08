#include <QString>
#include "module.h"
#include "kernel.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "domainitem.h"
#include "domain.h"
#include "numericdomain.h"
#include "itemdomain.h"
#include "range.h"
#include "numericrange.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "itemrange.h"
#include "datadefinition.h"

using namespace Ilwis;

DataDefinition::DataDefinition() : _range(0)
{
}


DataDefinition::DataDefinition(const DataDefinition& def)
{
    operator=(def);
}

DataDefinition::DataDefinition(const IDomain &dm, Range *rng)
{
    domain(dm);
    if ( rng)
        _range.reset(rng);
}

DataDefinition::~DataDefinition()
{
    _domain.set(0);
}

DataDefinition &DataDefinition::operator =(const DataDefinition &def)
{
    domain(def.domain<>());
    if ( !def.range().isNull())
        _range.reset(def.range()->clone());
    else
        _range.reset(0);

    return *this;
}

void DataDefinition::range(Range* vr)
{
    _range.reset(vr);
}

void DataDefinition::domain(const IDomain &dom)
{
    _domain = dom;
    if ( !_domain.isValid())
        return;

    SPRange r = _domain->range<>();
    if ( r)
        _range.reset(r->clone());

}


bool DataDefinition::isValid() const
{
    return _domain.isValid();
}

bool DataDefinition::isCompatibleWith(const DataDefinition &def) const
{
    if (!isValid())
        return false;

    return _domain->isCompatibleWith(def.domain<>().ptr());

}


DataDefinition DataDefinition::merge(const DataDefinition &def1, const DataDefinition &def2)
{
    IDomain dm;
    DataDefinition def;
    if ( !def1.isValid() && def2.isValid())
        return def2;
    if ( !def2.isValid() && def1.isValid())
        return def1;

    if ( def1.domain()->ilwisType() == itNUMERICDOMAIN && def1.domain()->ilwisType() == itNUMERICDOMAIN) {
        if ( def1.domain()->code() == "boolean" && def2.domain()->code() == "boolean") {
            def = DataDefinition(def1);
        }
        SPNumericRange nr1 = def1.range().dynamicCast<NumericRange>();
        SPNumericRange nr2 = def1.range().dynamicCast<NumericRange>();
        NumericRange *nrNew = NumericRange::merge(nr1, nr2);
        if ( def1.domain()->name() == def2.domain()->name()) {
            def = DataDefinition(def1.domain(), nrNew);
        } else {
            dm.prepare("value");
            def = DataDefinition(dm, nrNew);
        }
        return def;

    }
    if ( def1.domain()->ilwisType() == itITEMDOMAIN && def1.domain()->ilwisType() == itITEMDOMAIN) {
        SPItemRange nr1 = def1.range().dynamicCast<ItemRange>();
        SPItemRange nr2 = def1.range().dynamicCast<ItemRange>();
        ItemRange *nrNew = ItemRange::merge(nr1, nr2);
        if ( def1.domain()->name() == def2.domain()->name()) {
            return DataDefinition(def1.domain(), nrNew);
        } else {
            dm.prepare("blavla");
            return DataDefinition(dm, nrNew);
        }
    }
    return DataDefinition();
}

//-----------------------------------------------------------
bool Ilwis::operator==(const DataDefinition &def1, const DataDefinition &def2)
{
    return def1.domain() == def2.domain();
}

bool Ilwis::operator!=(const DataDefinition &def1, const DataDefinition &def2)
{
    return !operator==(def1, def2);
}
