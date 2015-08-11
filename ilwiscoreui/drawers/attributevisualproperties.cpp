#include "kernel.h"
#include <QColor>
#include "ilwisdata.h"
#include "domain.h"
#include "colorlookup.h"
#include "range.h"
#include "itemrange.h"
#include "colorrange.h"
#include "numericrange.h"
#include "representation.h"
#include "raster.h"
#include "uicontextmodel.h"
#include "attributevisualproperties.h"

using namespace Ilwis;
using namespace Geodrawer;

const QString VisualAttribute::LAYERATTRIBUTE="ILWIS_SINGLE_VALUE_LAYER_ATTRIBUTE";

VisualAttribute::VisualAttribute()
{
}

Ilwis::Geodrawer::VisualAttribute::VisualAttribute(const NumericRange &rng) : _actualRange(rng)
{
}

VisualAttribute::VisualAttribute(const IDomain &dom, int colindex, const IRepresentation& rpr) : _domain(dom)
{
    if ( rpr.isValid() && rpr->isCompatible(dom))
        _representation = rpr;
    else
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
    if ( _stretchRange.isValid())
        return _stretchRange;
    return _actualRange;
}

void VisualAttribute::stretchRange(const NumericRange &rng)
{
    _stretchRange = rng;
}

quint32 tovalue(const QString& name){
    short asnum = 0;
    for(auto c : name){
        asnum += c.unicode();
    }
    return asnum * name.size();
}

std::vector<QColor> VisualAttribute::colors(int size) const{
    if ( _representation.isValid() &&  _representation->colors())
        return _representation->colors()->values2colors(_actualRange, _stretchRange,size) ;
    return std::vector<QColor>();
}

QColor VisualAttribute::value2color(const QVariant &var) const
{
    if ( !_domain.isValid()){ // the range case
        bool ok;
        double d = var.toDouble(&ok);
        if ( ok){
            double f = (d - _actualRange.min())/_actualRange.distance();
            int defColorSize = ColorRangeBase::defaultColorNames().size();
            int index = std::max(0.0,std::min((double)defColorSize-1, f * (defColorSize)));
            return ColorRangeBase::defaultColor(index);
        }
    }
    double v;
    if ( hasType(_domain->valueType(), itCONTINUOUSCOLOR)){
        return ColorRangeBase::toColor(var.toULongLong(), ColorRangeBase::cmRGBA);
    }else if ( hasType(_domain->ilwisType(), itNUMERICDOMAIN | itITEMDOMAIN)){
        v  =  var.toDouble();
    }else{
        v = tovalue(var.toString());
    }

    return _representation->colors()->value2color(v, _actualRange, _stretchRange) ;
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


