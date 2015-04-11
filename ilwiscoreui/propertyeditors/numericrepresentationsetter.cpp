#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "representation.h"
#include "numericrepresentationsetter.h"
#include "mathhelper.h"
#include "drawers/drawerinterface.h"


RepresentationElement::RepresentationElement(QObject *parent) : QObject(parent)
{
}

RepresentationElement::RepresentationElement(QString label, QObject *parent) : QObject(parent), Identity(label)
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
        QVariant actAttribute = layer()->drawer()->attribute("activevisualattribute");
        if ( !actAttribute.isValid())
            return ;
        QVariant var = layer()->drawer()->attribute("visualattribute|representation|" + actAttribute.toString());
        _representation = var.value<IRepresentation>();
        var = layer()->drawer()->attribute("visualattribute|stretchrange|" + actAttribute.toString());
        NumericRange numrange = var.value<NumericRange>();
        if ( !numrange.isValid())
            return;

        NumericRange roundedRange = MathHelper::roundRange(numrange.min(), numrange.max());
        double tickValue = roundedRange.min();
        while(tickValue <= numrange.max()){
            _rprElements.push_back(new RepresentationElement(QString::number(tickValue),this));
            tickValue += roundedRange.resolution();
        }
        _rprElements.push_back(new RepresentationElement(QString::number(tickValue),this));

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

bool NumericRepresentationSetter::canUse(const QString &id) const
{
    try {
        bool ok;
        quint64 objid = id.toULongLong(&ok);
        if ( !ok)
            return false;
        Resource resource = mastercatalog()->id2Resource(objid);
        if ( !resource.isValid())
            return false;
        if ( resource.ilwisType() != itREPRESENTATION)
            return false;
        IRepresentation representation(resource);
        return hasType(representation->domain()->ilwisType(), itNUMERICDOMAIN);

    } catch(const ErrorObject& err){

    }
    return false;

}

void NumericRepresentationSetter::setRepresentation(const QString &name)
{
    Resource resource = mastercatalog()->name2Resource(name, itREPRESENTATION)    ;
    if ( !resource.isValid())
        return;
    IRepresentation rpr(resource);
    QVariant actAttribute = layer()->drawer()->attribute("activevisualattribute");
    if ( !actAttribute.isValid())
        return ;
    QVariant var;
    var.setValue<IRepresentation>(rpr);
    layer()->drawer()->setAttribute("visualattribute|representation|" + actAttribute.toString(),var);
    layer()->drawer()->unprepare(Geodrawer::DrawerInterface::ptGEOMETRY);
    layer()->drawer()->prepare(Geodrawer::DrawerInterface::ptGEOMETRY, IOOptions());
    layer()->drawer()->redraw();

}

QQmlListProperty<RepresentationElement> NumericRepresentationSetter::representationElements()
{
    return  QQmlListProperty<RepresentationElement>(this, _rprElements);
}

