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

NumericDomain::NumericDomain(const Resource& res) : Domain(res) {
    setStrict(false);
    setRange(new NumericRange(-1e300, 1e300));
}

void NumericDomain::setRange(Range *vr)
{
    _range = QSharedPointer<Range>(vr);
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

SPRange NumericDomain::getRange() const
{
    return _range;
}

QString NumericDomain::value(double v) const
{
    //TODO formatting according to domain(step size and valuetype, other specifiers belong at UI level).

    return QString::number(v)    ;
}

Domain::Containement NumericDomain::contains(const QString &value) const
{
    if ( !_range.isNull()) {
        bool ok;
        double v = value.toDouble(&ok);
        if (!ok)
            return Domain::cNONE;
        QSharedPointer<NumericRange> vr(_range.dynamicCast<NumericRange>());
        if ( !vr.isNull() && vr->contains(v))
            return Domain::cSELF;
        if (!ok && !isStrict() && parent().isValid())
            if (parent()->contains(value) == Domain::cSELF)
                return Domain::cPARENT;
    }
    return Domain::cNONE;
}





