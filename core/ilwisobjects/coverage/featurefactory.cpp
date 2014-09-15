#include "coverage.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "numericdomain.h"
#include "featurecoverage.h"
#include "feature.h"

using namespace Ilwis;

FeatureFactory::FeatureFactory() : AbstractFactory("FeatureFactory","ilwis", "Access to create functions for features")
{
}

void FeatureFactory::addCreator(const QString &type, CreateFeature crFunc)
{
    auto iter = _featureFactories.find(type);
    if ( iter == _featureFactories.end()) {
        _featureFactories[type] = crFunc;
    }
}

CreateFeature FeatureFactory::getCreator(const QString &type)
{
    auto iter = _featureFactories.find(type);
    if ( iter != _featureFactories.end())
        return (*iter).second;
    return 0;
}
