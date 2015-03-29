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
    VisualAttributeEditor("polygonpropertysetter",TR("Boundariees & Areas"),QUrl("PolygonProperties.qml"), parent)
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
    if ( !layer())
        return false;

    QVariant var = layer()->drawer()->attribute("polygonboundaries");
    if ( var.isValid())
        return var.toBool();
    return false;

}

void PolygonPropertySetter::setShowBoundaries(bool yesno)
{
    if ( !layer())
        return ;

    layer()->drawer()->setAttribute("polygonboundaries", yesno);
    layer()->drawer()->redraw();

}

bool PolygonPropertySetter::showAreas() const
{
    if ( !layer())
        return false;

    QVariant var = layer()->drawer()->attribute("polygonareas");
    if ( var.isValid())
        return var.toBool();
    return false;

}

void PolygonPropertySetter::setShowAreas(bool yesno)
{
    if ( !layer())
        return ;

    layer()->drawer()->setAttribute("polygonareas", yesno);
    layer()->drawer()->redraw();

}

VisualAttributeEditor *PolygonPropertySetter::create()
{
    return new PolygonPropertySetter();
}
