#include "coverage.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "colorlookup.h"
#include "representation.h"
#include "attributevisualproperties.h"
#include "spatialdatadrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

SpatialDataDrawer::SpatialDataDrawer(const QString &name, DrawerInterface *parentDrawer, RootDrawer *rootdrawer) : ComplexDrawer(name, parentDrawer, rootdrawer)
{
}

std::vector<double> SpatialDataDrawer::numericAttributeValues(const QString &attribute) const
{
    if (_coverage.isValid()){
        if ( hasType(_coverage->ilwisType(), itFEATURE)){
            IFeatureCoverage features = _coverage.as<FeatureCoverage>();
            auto columnValues = features->attributeTable()->column(attribute);
            std::vector<double> attribValues(columnValues.size());
            int count = 0;
            for(const auto& value : columnValues){
                bool ok;
                const double v = value.toDouble(&ok);
                attribValues[count++] = ok ? v : rUNDEF;
            }
            return attribValues;
        }
        return std::vector<double>();
    }
    return std::vector<double>();
}

ICoverage SpatialDataDrawer::coverage() const
{
    return _coverage;
}

void SpatialDataDrawer::coverage(const ICoverage &cov)
{
    _coverage = cov;
    _envelope = cov->envelope();
}

Envelope SpatialDataDrawer::envelope() const
{
    return _envelope;
}

void SpatialDataDrawer::envelope(const Envelope &env)
{
    _envelope = env;
}

AttributeVisualProperties SpatialDataDrawer::attribute(const QString &attrName) const
{
    auto iter = _visualProperties.find(attrName)    ;
    if ( iter != _visualProperties.end())
        return iter->second;
    return AttributeVisualProperties();
}

void SpatialDataDrawer::attribute(const QString &attrName, const AttributeVisualProperties &properties)
{
    _visualProperties[attrName] = properties;
}




