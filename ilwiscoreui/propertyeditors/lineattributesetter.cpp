#include "attributesetter.h"
#include "raster.h"
#include "featurecoverage.h"
#include "feature.h"
#include "table.h"
#include "lineattributesetter.h"

REGISTER_PROPERTYEDITOR("linepropertysetter",LinePropertySetter)

LinePropertySetter::LinePropertySetter(QObject *parent) :
    VisualAttributeEditor("linepropertysetter",TR("Line properties"),QUrl("propertyeditors/LineStyleSetter.qml"), parent)
{
    setAttributeDependent(true);
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

VisualAttributeEditor *LinePropertySetter::create()
{
    return new LinePropertySetter();
}
