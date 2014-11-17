#include "coverage.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "spatialdatadrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

SpatialDataDrawer::SpatialDataDrawer(const QString &name, DrawerInterface *parentDrawer, RootDrawer *rootdrawer) : ComplexDrawer(name, parentDrawer, rootdrawer)
{
}

void SpatialDataDrawer::setCoverage(const ICoverage &coverage)
{
    _coverage = coverage;
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

NumericRange SpatialDataDrawer::stretchRange()
{
    if ( _stretchRange.isValid()){
        return _stretchRange;
    }
    return NumericRange();
}

void SpatialDataDrawer::stretchRange(const NumericRange &numrange)
{
    _stretchRange = numrange;
}

bool SpatialDataDrawer::isStretched() const
{
    return _stretched;
}

void SpatialDataDrawer::stretched(bool yesno)
{
    _stretched = yesno;
}


