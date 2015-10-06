#include <QColor>
#include <QSharedPointer>
#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "datadefinition.h"
#include "itemrange.h"
#include "domainitem.h"
#include "coloritem.h"
#include "colorrange.h"

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
        return _range->valueType();
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

bool ColorDomain::isCompatibleWith(const IlwisObject* obj, bool strict) const
{
    if ( !obj || !obj->isValid())
        return false;
    if ( obj->ilwisType() != itCOLORDOMAIN)
        return false;
    return true;
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
        if ( colorrange->valueType() == itPALETTECOLOR){
            if( cNONE == parent()->range<ColorPalette>()->contains(colorrange))
                return;
        } else if ( colorrange->valueType() == itCONTINUOUSCOLOR){
            if( cNONE == parent()->range<ContinuousColorRange>()->contains(colorrange))
                return;
        }
    }
    _range = QSharedPointer<Range>(colorrange);
}

bool ColorDomain::isOrdered() const
{
    return false;
}

QSharedPointer<Range> ColorDomain::getRange() const
{
    return _range;
}

void ColorDomain::setParent(const IDomain& dm){

}

IlwisObject *ColorDomain::clone()
{
    auto *colordom = new ColorDomain();
    copyTo(colordom);
    return colordom;
}

void ColorDomain::copyTo(IlwisObject *obj){
    Domain::copyTo(obj);
    ColorDomain *colordom = static_cast<ColorDomain *>(obj);
    colordom->_range.reset( _range->clone());
}


