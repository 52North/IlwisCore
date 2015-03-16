#include "attributesetter.h"
#include "raster.h"
#include "featurecoverage.h"
#include "feature.h"
#include "table.h"
#include "polygonattributesetter.h"

REGISTER_PROPERTYEDITOR("polygonpropertysetter",PolygonPropertySetter)

PolygonPropertySetter::PolygonPropertySetter(QObject *parent) :
    VisualAttributeEditor("polygonpropertysetter",TR("Polygon properties"),QUrl("PolygonProperties.qml"), parent)
{

}

PolygonPropertySetter::~PolygonPropertySetter()
{

}

bool PolygonPropertySetter::canUse(const IIlwisObject &obj, const DataDefinition& ) const
{
    if ( !obj.isValid())
        return false;

    if ( hasType(obj->ilwisType(), itFEATURE)){
        IFeatureCoverage features = obj.as<FeatureCoverage>();
        return features->featureCount(itPOLYGON) > 0;
    }
    return false;
}

VisualAttributeEditor *PolygonPropertySetter::create()
{
    return new PolygonPropertySetter();
}
