#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "attributesetter.h"
#include "raster.h"
#include "featurecoverage.h"
#include "feature.h"
#include "polygonattributesetter.h"
#include "visualattributemodel.h"

REGISTER_PROPERTYEDITOR("polygonpropertysetter",PolygonPropertySetter)

PolygonPropertySetter::PolygonPropertySetter(QObject *parent) :
    VisualAttributeEditor("polygonpropertysetter",TR("Boundaries & Areas"),QUrl("PolygonProperties.qml"), parent)
{

}

PolygonPropertySetter::~PolygonPropertySetter()
{

}

bool PolygonPropertySetter::canUse(const IIlwisObject& obj, const QString& name ) const
{
    if (!obj.isValid())
        return false;
    if(!hasType(obj->ilwisType(), itFEATURE))
        return false;
    Ilwis::IFeatureCoverage features = obj.as<Ilwis::FeatureCoverage>();
    if ( features->featureCount(itPOLYGON) == 0)
        return false;
    return name == VisualAttributeModel::LAYER_ONLY;

}

bool PolygonPropertySetter::showBoundaries() const
{
    if ( !attribute()->layer())
        return false;

    QVariant var = attribute()->layer()->drawer()->attribute("polygonboundaries");
    if ( var.isValid())
        return var.toBool();
    return false;

}

void PolygonPropertySetter::setShowBoundaries(bool yesno)
{
    if ( !attribute()->layer())
        return ;

    attribute()->layer()->drawer()->setAttribute("polygonboundaries", yesno);
    attribute()->layer()->drawer()->redraw();

}

bool PolygonPropertySetter::showAreas() const
{
    if ( !attribute()->layer())
        return false;

    QVariant var = attribute()->layer()->drawer()->attribute("polygonareas");
    if ( var.isValid())
        return var.toBool();
    return false;

}

void PolygonPropertySetter::setShowAreas(bool yesno)
{
    if ( !attribute()->layer())
        return ;

    attribute()->layer()->drawer()->setAttribute("polygonareas", yesno);
    attribute()->layer()->drawer()->redraw();

}

QColor PolygonPropertySetter::boundaryColor() const
{
    if ( attribute()->layer()){
        QVariant var = attribute()->layer()->drawer()->attribute("boundarycolor");
        if ( var.isValid())
            return var.value<QColor>();
    }
    return QColor();

}

void PolygonPropertySetter::setBoundaryColor(const QColor &clr)
{
    if ( !attribute()->layer())
        return ;

    attribute()->layer()->drawer()->setAttribute("boundarycolor", clr);
    attribute()->layer()->drawer()->unprepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER);
    attribute()->layer()->drawer()->prepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER, Ilwis::IOOptions("polygononly",true));
    attribute()->layer()->drawer()->redraw();
}

float PolygonPropertySetter::boundarywidth() const
{
    if ( !attribute()->layer())
        return 1.0;
    QVariant var = attribute()->layer()->drawer()->attribute("boundarywidth");
    if ( var.isValid())
        return var.toInt();
    return 1.0;

}

void PolygonPropertySetter::setBoundarywidth(float w)
{
    if ( !attribute()->layer())
        return ;

    attribute()->layer()->drawer()->setAttribute("boundarywidth", w);
    attribute()->layer()->drawer()->unprepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER);
    attribute()->layer()->drawer()->prepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER, Ilwis::IOOptions("polygononly",true));
    attribute()->layer()->drawer()->redraw();
}

VisualAttributeEditor *PolygonPropertySetter::create()
{
    return new PolygonPropertySetter();
}
