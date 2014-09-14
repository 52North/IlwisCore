#ifndef FEATUREFACTORY_H
#define FEATUREFACTORY_H

#include <functional>
#include <map>

namespace Ilwis {
class FeatureInterface;
class FeatureCoverage;

typedef IlwisData<FeatureCoverage> IFeatureCoverage;

typedef std::function<FeatureInterface *(const IFeatureCoverage& fcoverage, quint8)>  CreateFeature;

class FeatureFactory : public AbstractFactory
{
public:
    FeatureFactory();
    void addCreator(const QString& type, CreateFeature crFunc);
    CreateFeature getCreator(const QString& type);
private:
    std::map<QString, CreateFeature> _featureFactories;


};
}

#endif // FEATUREFACTORY_H
