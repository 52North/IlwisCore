#include <QUrl>
#include "kernel.h"
#include "connectorinterface.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "numericrange.h"
#include "numericdomain.h"

using namespace Ilwis;

NumericDomain::NumericDomain()
{
    setStrict(false);
    setRange(new NumericRange(-1e300, 1e300));

}

NumericDomain::NumericDomain(const Resource& resource) : Domain(resource) {
    setStrict(false);
    setRange(new NumericRange(-1e300, 1e300));
}

void NumericDomain::setRange(Range *vr)
{
    if ( hasType(vr->valueType(), itNUMBER) == false)
        return;
    if ( parent().isValid()) {
        parent()->range<NumericRange>()->contains(static_cast<NumericRange *>(vr));
    }
    _range = QSharedPointer<NumericRange>(static_cast<NumericRange *>(vr));
}

IlwisTypes NumericDomain::valueType() const
{
    if ( !_range.isNull())
        return _range->valueType();

    return itUNKNOWN;
}

IlwisTypes NumericDomain::ilwisType() const
{
    return itNUMERICDOMAIN;
}

void NumericDomain::setParent(const IDomain &dm)
{
    if ( _range.isNull()) {
        return;
    }
    if ( dm->ilwisType() != itNUMERICDOMAIN || dm->valueType() != itNUMBER)
        return;
    SPNumericRange numrange = dm->range<NumericRange>();
    if ( !numrange->contains(_range))
        return;

    Domain::setParent(dm);
}

SPRange NumericDomain::getRange() const
{
    return _range;
}

QString NumericDomain::value(const QVariant &v) const
{
    //TODO formatting according to domain(step size and valuetype, other specifiers belong at UI level).

    return v.toString();
}

Domain::Containement NumericDomain::contains(const QVariant &value) const
{
    if ( !_range.isNull()) {
        bool ok;
        double v = value.toDouble(&ok);
        if (!ok)
            return Domain::cNONE;
        QSharedPointer<NumericRange> vr(_range.dynamicCast<NumericRange>());
        if ( !vr.isNull() && vr->contains(v))
            return Domain::cSELF;
        if (!ok && parent().isValid())
            if (parent()->contains(value) == Domain::cSELF)
                return Domain::cPARENT;
    }
    return Domain::cNONE;
}

bool NumericDomain::isCompatibleWith(const IDomain& dom) const {
    if ( !dom->isValid())
        return false;
    if(dom->ilwisType() != itNUMERICDOMAIN)
        return false;
    return true;
}







