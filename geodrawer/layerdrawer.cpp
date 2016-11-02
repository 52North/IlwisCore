#include "coverage.h"
#include "range.h"
#include "itemrange.h"
#include "colorrange.h"
#include "colorlookup.h"
#include "representation.h"
#include "drawers/attributevisualproperties.h"
#include "table.h"
#include "layerdrawer.h"
#include "drawingcolor.h"
#include <QtGui/QOpenGLContext>
#include "drawers/drawerfactory.h"

using namespace Ilwis;
using namespace Geodrawer;

LayerDrawer::LayerDrawer(const QString& name, DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options) :
    SpatialDataDrawer(name, parentDrawer, rootdrawer, options),
    _activeAttribute(COVERAGEKEYCOLUMN)
{
    valid(true); //TODO: this must move to the prepare once the correct call sequences is in place
}

QString LayerDrawer::activeAttribute() const
{
    return _activeAttribute;
}

void LayerDrawer::setAttribute(const QString &key, const QVariant &attribValue)
{
    SpatialDataDrawer::setAttribute(key, attribValue);
    if ( key == "activevisualattribute"){
        setActiveVisualAttribute(attribValue.toString());
    }
}

QVariant LayerDrawer::attribute(const QString &key) const
{
    QVariant var = SpatialDataDrawer::attribute(key);
    if ( var.isValid())
        return var;

    if ( key == "activevisualattribute"){
        return _activeAttribute;
    }
    return QVariant();
}

QColor LayerDrawer::color(const IRepresentation &rpr, double value, DrawerInterface::ColorValueMeaning cvm)
{
    if ( _activeAttribute != sUNDEF){
        VisualAttribute& attr = _visualProperties[_activeAttribute];
        if ( cvm ==DrawerInterface::cvmFRACTION){
            NumericRange numrange = attr.stretchRange();
            if ( !numrange.isValid()){
                numrange = attr.actualRange();
            }
            value = numrange.min() + numrange.distance() * value;

        }
        QColor clr = attr.value2color(value);
        return clr;
    }
    return QColor();
}

void LayerDrawer::setActiveVisualAttribute(const QString &attr)
{
    if ( !isVisualProperty(attr)){
        return;
    }
    _activeAttribute = attr;
}

void LayerDrawer::cleanUp()
{
}




