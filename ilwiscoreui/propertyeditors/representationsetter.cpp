#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "mastercatalog.h"
#include "representationsetter.h"

using namespace Ilwis;

REGISTER_PROPERTYEDITOR(itCOVERAGE,"representationeditor",RepresentationSetter)

RepresentationElement::RepresentationElement(QObject *parent) : QObject(parent)
{
}

QColor RepresentationElement::color() const
{
    return _color;
}
//-------------------------------------------------
PropertyEditor *RepresentationSetter::create()
{
    return new RepresentationSetter();
}

RepresentationSetter::RepresentationSetter(QObject *parent) : PropertyEditor("representationeditor",QUrl("RepresentationProperties.qml"), parent)
{
}

QQmlListProperty<RepresentationElement> RepresentationSetter::representationElements()
{
    if ( _rprElements.size() == 0){
        if ( hasType(layer()->type(), itCOVERAGE)){

        }
    }
    return  QQmlListProperty<RepresentationElement>(this, _rprElements);
}

QString RepresentationSetter::activeValueType() const
{
    QVariant var = layer()->drawer()->attribute("activevisualattribute");
    if ( !var.isValid())
        return "";
    var = layer()->drawer()->attribute("visualattribute|domain|" + var.toString());
    Ilwis::IDomain dom = var.value<IDomain>();
    if ( !dom.isValid())
        return "";
    if ( hasType(dom->valueType(), itNUMBER))
        return "number";
    if ( hasType(dom->valueType(), itTHEMATICITEM|itNUMERICITEM|itTIMEITEM))
        return "item";
    return "";
}

QString RepresentationSetter::representationName() const
{
    if ( _representation.isValid())
        return _representation->name();
    return sUNDEF;
}

QColor RepresentationSetter::color(double frac)
{
    return layer()->drawer()->color(_representation, frac, Ilwis::Geodrawer::DrawerInterface::cvmFRACTION) ;
}

QColor RepresentationSetter::color2(const QString &clr) const
{
    QColor qclr(clr);
    return qclr;
}

void RepresentationSetter::setlayer(CoverageLayerModel *model)
{
    PropertyEditor::setlayer(model);

    QVariant var = layer()->drawer()->attribute("activevisualattribute");
    if ( !var.isValid())
        return ;
    var = layer()->drawer()->attribute("visualattribute|representation|" + var.toString());
    _representation = var.value<IRepresentation>();

    emit rprNameChanged();
}







