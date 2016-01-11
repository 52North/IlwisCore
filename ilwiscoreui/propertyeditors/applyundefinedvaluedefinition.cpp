#include "kernel.h"
#include "raster.h"
#include "models/visualattributemodel.h"
#include "applyundefinedvaluedefinition.h"


REGISTER_PROPERTYEDITOR("applyundefinedvalue",ApplyUndefinedValueDefinition)

ApplyUndefinedValueDefinition::ApplyUndefinedValueDefinition(QObject *parent) :
    VisualAttributeEditor("applyundefinedvalue",TR("Set Undefined value"),QUrl("UndefinedValueSetter.qml"), parent)
{

}

bool ApplyUndefinedValueDefinition::canUse(const IIlwisObject &obj, const QString &name) const
{
    if (!obj.isValid())
        return false;
    if ( obj->ilwisType() == itRASTER)   {
        Ilwis::IRasterCoverage raster = obj.as<RasterCoverage>();
        return hasType(raster->datadef().domain()->ilwisType(), itNUMERICDOMAIN);
    }
    return false;
}

VisualAttributeEditor *ApplyUndefinedValueDefinition::create()
{
    return new ApplyUndefinedValueDefinition();
}

double ApplyUndefinedValueDefinition::undefinedvalue() const
{
    if ( attribute()->layer())
        return attribute()->layer()->drawer()->attribute("undefinedvalue").toFloat();
    return rUNDEF;
}

void ApplyUndefinedValueDefinition::undefinedvalue(double tr)
{
    if (!attribute()->layer())
        return;

    attribute()->layer()->drawer()->setAttribute("undefinedvalue", tr);
    attribute()->layer()->drawer()->unprepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER);
    attribute()->layer()->drawer()->prepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER, IOOptions());
    attribute()->layer()->drawer()->redraw();

}

