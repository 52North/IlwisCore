#include <QColor>
#include <QSharedPointer>
#include "kernel.h"
#include "range.h"
#include "colorrange.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "colordomain.h"

using namespace Ilwis;



ColorDomain::ColorDomain()
{
}

ColorDomain::ColorDomain(const Resource &resource) : Domain(resource)
{

}

IlwisTypes ColorDomain::valueType() const
{
    if ( !_range.isNull())
        _range->valueType();
    return itUNKNOWN;
}

QVariant ColorDomain::impliedValue(const QVariant &value) const
{
    if ( !_range.isNull())
        return _range->impliedValue(value);
    return QVariant();
}

Domain::Containement ColorDomain::contains(const QVariant &value) const
{
    if ( !_range.isNull()) {
        if ( !_range.isNull() && _range->contains(value))
            return Domain::cSELF;
        if (parent().isValid())
            if (parent()->contains(value) == Domain::cSELF)
                return Domain::cPARENT;
    }
    return Domain::cNONE;
}

bool ColorDomain::isCompatibleWith(const IDomain &dom) const
{
    if ( !dom.isValid())
        return false;
    return dom->valueType() == valueType() && dom->ilwisType() == ilwisType();
}

IlwisTypes ColorDomain::ilwisType() const
{
    return itCOLORDOMAIN;
}

void ColorDomain::range(Range *colorrange)
{
    if ( isReadOnly())
        return;
    changed(true);
    if ( hasType(colorrange->valueType(), itCOLOR) == false)
        return;
    if ( parent().isValid()) {
        parent()->range2range<ColorRange>()->contains(static_cast<ColorRange *>(colorrange));
    }
    _range = QSharedPointer<ColorRange>(static_cast<ColorRange *>(colorrange));
}

QSharedPointer<Range> ColorDomain::getRange() const
{
    return _range;
}


