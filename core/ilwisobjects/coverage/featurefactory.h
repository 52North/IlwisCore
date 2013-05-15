#ifndef FEATUREFACTORY_H
#define FEATUREFACTORY_H

#include <functional>
#include <map>

namespace Ilwis {
class Feature;

typedef std::function<Feature *(quint64 itemId)>  CreateFeature;

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
