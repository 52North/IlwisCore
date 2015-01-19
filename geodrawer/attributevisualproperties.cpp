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

AttributeVisualProperties::AttributeVisualProperties()
{
}

AttributeVisualProperties::AttributeVisualProperties(const IDomain &dom) : _domain(dom)
{
    _representation = Representation::defaultRepresentation(dom);
}

AttributeVisualProperties::AttributeVisualProperties(const AttributeVisualProperties &avp)
{
    _representation = avp._representation;
    _actualRange = avp._actualRange;
    _stretchRange = avp._stretchRange;
    _domain = avp._domain;
}

IRepresentation AttributeVisualProperties::representation() const
{
    return _representation;
}

void AttributeVisualProperties::representation(const IRepresentation &rpr)
{
    if ( !_domain.isValid() || !rpr.isValid())
        return;
    if ( rpr->isCompatible(_domain)){
        _representation = rpr;
    }
}

IDomain AttributeVisualProperties::domain() const
{
    return _domain;
}

void AttributeVisualProperties::domain(const IDomain &dom)
{
    if ( _representation.isValid() && !_representation->isCompatible(dom)){
        _representation = IRepresentation();
    }
    _domain = dom;
}

NumericRange AttributeVisualProperties::stretchRange() const
{
    return _stretchRange;
}

void AttributeVisualProperties::stretchRange(const NumericRange &rng)
{
    _stretchRange = rng;
}

QColor AttributeVisualProperties::value2color(const QVariant &var)
{
    return _representation->colors()->value2color(var.toDouble(), _actualRange, _stretchRange) ;
}

NumericRange AttributeVisualProperties::actualRange() const
{
    return _actualRange;
}

void AttributeVisualProperties::actualRange(const NumericRange &rng)
{
    _actualRange = rng;
}


