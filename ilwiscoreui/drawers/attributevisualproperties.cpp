#include "kernel.h"
#include <QColor>
#include "ilwisdata.h"
#include "domain.h"
#include "colorlookup.h"
#include "range.h"
#include "numericrange.h"
#include "representation.h"
#include "attributevisualproperties.h"

using namespace Ilwis;
using namespace Geodrawer;

VisualAttribute::VisualAttribute()
{
}

VisualAttribute::VisualAttribute(const IDomain &dom, int colindex) : _domain(dom)
{
    _representation = Representation::defaultRepresentation(dom);
    _representation->domain(dom);
    _domain = dom;
    _columnIndex = colindex;
}

VisualAttribute::VisualAttribute(const VisualAttribute &avp)
{
    _representation = avp._representation;
    _actualRange = avp._actualRange;
    _stretchRange = avp._stretchRange;
    _domain = avp._domain;
    _columnIndex = avp._columnIndex;
}

IRepresentation VisualAttribute::representation() const
{
    return _representation;
}

void VisualAttribute::representation(const IRepresentation &rpr)
{
    if ( !_domain.isValid() || !rpr.isValid())
        return;
    if ( rpr->isCompatible(_domain)){
        _representation = rpr;
    }
}

IDomain VisualAttribute::domain() const
{
    return _domain;
}

void VisualAttribute::domain(const IDomain &dom)
{
    if ( _representation.isValid() && !_representation->isCompatible(dom)){
        _representation = IRepresentation();
    }
    _domain = dom;
}

NumericRange VisualAttribute::stretchRange() const
{
    return _stretchRange;
}

void VisualAttribute::stretchRange(const NumericRange &rng)
{
    _stretchRange = rng;
}

QColor VisualAttribute::value2color(const QVariant &var) const
{
    return _representation->colors()->value2color(var.toDouble(), _actualRange, _stretchRange) ;
}

NumericRange VisualAttribute::actualRange() const
{
    return _actualRange;
}

void VisualAttribute::actualRange(const NumericRange &rng)
{
    _actualRange = rng;
}

quint32 VisualAttribute::columnIndex() const
{
    return _columnIndex;
}

void VisualAttribute::setColumnIndex(quint32 index)
{
    _columnIndex = index;
}


