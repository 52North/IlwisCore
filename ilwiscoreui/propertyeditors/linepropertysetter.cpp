#include "attributesetter.h"
#include "raster.h"
#include "featurecoverage.h"
#include "feature.h"
#include "table.h"
#include "linepropertysetter.h"

REGISTER_PROPERTYEDITOR("linepropertysetter",LinePropertySetter)

LinePropertySetter::LinePropertySetter(QObject *parent) :
    PropertyEditor("linepropertysetter",TR("Line properties"),QUrl("LineProperties.qml"), parent)
{

}

LinePropertySetter::~LinePropertySetter()
{

}

bool LinePropertySetter::canUse(const IIlwisObject &obj) const
{
    if ( !obj.isValid())
        return false;

    if ( hasType(obj->ilwisType(), itFEATURE)){
        IFeatureCoverage features = obj.as<FeatureCoverage>();
        return features->featureCount(itLINE) > 0;
    }
    return false;
}

PropertyEditor *LinePropertySetter::create()
{
    return new LinePropertySetter();
}
