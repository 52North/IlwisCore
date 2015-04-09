#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "numericrepresentationsetter.h"


RepresentationElement::RepresentationElement(QObject *parent) : QObject(parent)
{
}

QColor RepresentationElement::color() const
{
    return _color;
}

//-----------------------------------------
REGISTER_PROPERTYEDITOR("numericrepresentationsetter",NumericRepresentationSetter)

NumericRepresentationSetter::NumericRepresentationSetter(QObject *parent) :
        VisualAttributeEditor("numericrepresentationsetter",TR("Legend"),QUrl("NumericRepresentation.qml"), parent)
{

}

NumericRepresentationSetter::~NumericRepresentationSetter()

{

}

bool NumericRepresentationSetter::canUse(const IIlwisObject &obj, const ColumnDefinition &coldef) const
{
    if ( coldef.isValid())
        return hasType(coldef.datadef().domain()->ilwisType(), itNUMERICDOMAIN);
    return false;
}

VisualAttributeEditor *NumericRepresentationSetter::create()
{
    return new NumericRepresentationSetter();
}

void NumericRepresentationSetter::prepare(CoverageLayerModel *parentLayer, const IIlwisObject &obj, const ColumnDefinition &cdef)
{
    VisualAttributeEditor::prepare(parentLayer, obj, cdef);

    if ( layer() && layer()->drawer()){
        QVariant var = layer()->drawer()->attribute("activevisualattribute");
        if ( !var.isValid())
            return ;
        var = layer()->drawer()->attribute("visualattribute|representation|" + var.toString());
        _representation = var.value<IRepresentation>();

        emit rprNameChanged();
    }
}

QString NumericRepresentationSetter::representationName() const
{
    if ( _representation.isValid())
        return _representation->name();
    return "";
}

QColor NumericRepresentationSetter::color(double frac)
{
    if ( layer() && layer()->drawer()){
        return layer()->drawer()->color(_representation, frac, Ilwis::Geodrawer::DrawerInterface::cvmFRACTION) ;
    }
    return QColor();
}

QQmlListProperty<RepresentationElement> NumericRepresentationSetter::representationElements()
{
    return  QQmlListProperty<RepresentationElement>(this, _rprElements);
}

