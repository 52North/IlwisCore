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

DataDefinition::DataDefinition() : _coveragerange(0), _indexrange(0)
{
}


DataDefinition::DataDefinition(const DataDefinition& def)
{
    domain(def.domain());
    if ( !def.range().isNull())
        _coveragerange.reset(def.range()->clone());
    else
        _coveragerange.reset(0);

    domain(def.domain(daINDEX), daINDEX);
    if ( !def.range(daINDEX).isNull())
        _coveragerange.reset(def.range(daINDEX)->clone());
    else
        _coveragerange.reset(0);
}

DataDefinition::DataDefinition(const IDomain &dm, Range *rng)
{
    domain(dm);
    if ( rng)
        _coveragerange.reset(rng);
}

DataDefinition::~DataDefinition()
{
}

DataDefinition &DataDefinition::operator =(const DataDefinition &def1)
{
    domain(def1.domain());
    domain(def1.domain(daINDEX), daINDEX);
    _coveragerange = def1.range(daCOVERAGE);
    _indexrange = def1.range(daINDEX);


    return *this;
}

Ilwis::SPRange DataDefinition::range(DomainAxis da) const
{
    if ( da == daCOVERAGE){
        return _coveragerange;
    }
    return _indexrange;
}

void DataDefinition::range(Range* vr, DomainAxis da)
{
    if ( da == daCOVERAGE)
        _coveragerange = QSharedPointer<Range>(vr);
    else
        _indexrange = QSharedPointer<Range>(vr);
}

IDomain DataDefinition::domain(DomainAxis da) const
{
    if ( da == daCOVERAGE)
        return _coveragedomain;
    return _indexdomain;
}

void DataDefinition::domain(const IDomain &dom, DomainAxis da)
{
    if ( da == daCOVERAGE) {
        _coveragedomain = dom;
        if ( !_coveragedomain.isValid())
            return;

        SPRange r = _coveragedomain->range<>();
        if ( r)
            _coveragerange.reset(r->clone());
    } else {
        _indexdomain = dom;
        if ( !_indexdomain.isValid())
            return;

        SPRange r = _indexdomain->range<>();
        if ( r)
            _indexrange.reset(r->clone());

    }

}


bool DataDefinition::isValid() const
{
    return _coveragedomain.isValid();
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
        if ( def1.domain()->name() == def1.domain()->name()) {
            def =  DataDefinition(def1.domain(), nrNew);
        } else {
            dm.prepare("value");
            def = DataDefinition(dm, nrNew);
        }
        bool def1Valid = def1.domain(daINDEX).isValid();
        bool def2Valid = def2.domain(daINDEX).isValid();
        if ( !def2Valid && !def1Valid)
            return def;
        if ( def1Valid && def2Valid) {
            SPNumericRange nr1 = def1.range(daINDEX).dynamicCast<NumericRange>();
            SPNumericRange nr2 = def1.range(daINDEX).dynamicCast<NumericRange>();
            NumericRange *nrNew = NumericRange::merge(nr1, nr2);
            if ( def1.domain(daINDEX)->name() == def1.domain(daINDEX)->name()) {
                def.domain(def1.domain(daINDEX));
            } else {
                dm.prepare("value");
                def.domain(dm, daINDEX);
            }
            def.range(nrNew, daINDEX);
        } else {
            DataDefinition defTemp = def1.domain(daINDEX).isValid() ? def1 : def2;
            def.range(defTemp.range(daINDEX)->clone(), daINDEX);
            def.domain(defTemp.domain(daINDEX), daINDEX);
        }
        return def;

    }
    if ( def1.domain()->ilwisType() == itITEMDOMAIN && def1.domain()->ilwisType() == itITEMDOMAIN) {
        SPItemRange nr1 = def1.range().dynamicCast<ItemRange>();
        SPItemRange nr2 = def1.range().dynamicCast<ItemRange>();
        ItemRange *nrNew = ItemRange::merge(nr1, nr2);
        if ( def1.domain()->name() == def1.domain()->name()) {
            return DataDefinition(def1.domain(), nrNew);
        } else {
            dm.prepare("blavla");
            return DataDefinition(dm, nrNew);
        }
    }
    return DataDefinition();
}




