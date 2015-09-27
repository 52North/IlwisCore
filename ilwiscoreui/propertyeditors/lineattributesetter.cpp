#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "attributesetter.h"
#include "raster.h"
#include "featurecoverage.h"
#include "feature.h"
#include "table.h"
#include "lineattributesetter.h"
#include "models/visualattributemodel.h"

REGISTER_PROPERTYEDITOR("linepropertysetter",LinePropertySetter)

LinePropertySetter::LinePropertySetter(QObject *parent) :
    VisualAttributeEditor("linepropertysetter",TR("Line properties"),QUrl("LineStyleSetter.qml"), parent)
{
}

LinePropertySetter::~LinePropertySetter()
{

}

bool LinePropertySetter::canUse(const IIlwisObject &obj, const QString& name) const
{
    if ( !obj.isValid())
        return false;

    if ( hasType(obj->ilwisType(), itFEATURE)){
        IFeatureCoverage features = obj.as<FeatureCoverage>();
        return (name == VisualAttributeModel::LAYER_ONLY) && (features->featureCount(itLINE) || features->featureCount(itPOLYGON)> 0);
    }
    return false;
}

VisualAttributeEditor *LinePropertySetter::create()
{
    return new LinePropertySetter();
}

bool LinePropertySetter::showBoundaries() const
{
    if ( !attribute()->layer())
        return false;

    QVariant var = attribute()->layer()->drawer()->attribute("polygonboundaries");
    if ( var.isValid())
        return var.toBool();
    return false;

}

void LinePropertySetter::setShowBoundaries(bool yesno)
{
    if ( !attribute()->layer())
        return ;

    attribute()->layer()->drawer()->setAttribute("polygonboundaries", yesno);
    attribute()->layer()->drawer()->redraw();

}

void LinePropertySetter::setBoundaryColor(const QColor &clr)
{
    if ( !attribute()->layer())
        return ;

    attribute()->layer()->drawer()->setAttribute("boundarycolor", clr);
    attribute()->layer()->drawer()->unprepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER);
    attribute()->layer()->drawer()->prepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER, Ilwis::IOOptions("polygononly",true));
    attribute()->layer()->drawer()->redraw();
}

QColor LinePropertySetter::boundaryColor() const
{
    if ( attribute()->layer()){
        QVariant var = attribute()->layer()->drawer()->attribute("boundarycolor");
        if ( var.isValid())
            return var.value<QColor>();
    }
    return QColor();

}

float LinePropertySetter::boundarywidth() const
{
    if ( !attribute()->layer())
        return 1.0;
    QVariant var = attribute()->layer()->drawer()->attribute("boundarywidth");
    if ( var.isValid())
        return var.toInt();
    return 1.0;

}

void LinePropertySetter::setBoundarywidth(float w)
{
    if ( !attribute()->layer())
        return ;

    attribute()->layer()->drawer()->setAttribute("boundarywidth", w);
    attribute()->layer()->drawer()->unprepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER);
    attribute()->layer()->drawer()->prepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER, Ilwis::IOOptions("polygononly",true));
    attribute()->layer()->drawer()->redraw();
}

