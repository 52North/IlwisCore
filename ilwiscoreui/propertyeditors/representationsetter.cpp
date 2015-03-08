#include "kernel.h"
#include "ilwisdata.h"
#include "raster.h"
#include "domain.h"
#include "mastercatalog.h"
#include "representationsetter.h"

using namespace Ilwis;

//REGISTER_PROPERTYEDITOR("representationeditor",RepresentationSetter)

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

RepresentationSetter::RepresentationSetter(QObject *parent) : PropertyEditor("representationeditor","dummy", QUrl("RepresentationProperties.qml"), parent)
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
    if ( layer() && layer()->drawer()){
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
    }
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
    if ( layer() && layer()->drawer()){
        return layer()->drawer()->color(_representation, frac, Ilwis::Geodrawer::DrawerInterface::cvmFRACTION) ;
    }
    return QColor();
}

QColor RepresentationSetter::name2color(const QString &clr) const
{
    QColor qclr(clr);
    return qclr;
}

void RepresentationSetter::setlayer(quint32 index, CoverageLayerModel *model)
{
    PropertyEditor::setlayer(index, model);

    if ( layer() && layer()->drawer()){
        QVariant var = layer()->drawer()->attribute("activevisualattribute");
        if ( !var.isValid())
            return ;
        var = layer()->drawer()->attribute("visualattribute|representation|" + var.toString());
        _representation = var.value<IRepresentation>();

        emit rprNameChanged();
    }
}

int RepresentationSetter::defaultHeight() const
{
    return 200;
}

bool RepresentationSetter::canUse(const Ilwis::IIlwisObject& obj) const{
    if ( obj->ilwisType() != itRASTER)
        return false;
    return true;

}







