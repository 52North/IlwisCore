#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "representation.h"
#include "numericrepresentationsetter.h"
#include "mathhelper.h"
#include "visualattributemodel.h"
#include "drawers/drawerinterface.h"


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

void NumericRepresentationSetter::prepare(VisualAttributeModel *vattrib, const IIlwisObject &obj, const ColumnDefinition &cdef)
{
    VisualAttributeEditor::prepare(vattrib, obj, cdef);

    if ( attribute()->layer() && attribute()->layer()->drawer()){
        QVariant actAttribute = attribute()->layer()->drawer()->attribute("activevisualattribute");
           if ( !actAttribute.isValid())
               return ;
        auto var = attribute()->layer()->drawer()->attribute("visualattribute|stretchrange|" + actAttribute.toString());
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

QColor NumericRepresentationSetter::color(double frac)
{
    if ( attribute()->layer() && attribute()->layer()->drawer()){
        return attribute()->layer()->drawer()->color(attribute()->representation(), frac, Ilwis::Geodrawer::DrawerInterface::cvmFRACTION) ;
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
//    Resource resource = mastercatalog()->name2Resource(name, itREPRESENTATION)    ;
//    if ( !resource.isValid())
//        return;
//    IRepresentation rpr(resource);
//    QVariant actAttribute = layer()->drawer()->attribute("activevisualattribute");
//    if ( !actAttribute.isValid())
//        return ;
//    QVariant var;
//    var.setValue<IRepresentation>(rpr);
//    layer()->drawer()->setAttribute("visualattribute|representation|" + actAttribute.toString(),var);
//    layer()->drawer()->unprepare(Geodrawer::DrawerInterface::ptRENDER);
//    layer()->drawer()->prepare(Geodrawer::DrawerInterface::ptRENDER, IOOptions());
//    layer()->drawer()->redraw();

}

QQmlListProperty<RepresentationElement> NumericRepresentationSetter::representationElements()
{
    return  QQmlListProperty<RepresentationElement>(this, _rprElements);
}

