#include "kernel.h"
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





