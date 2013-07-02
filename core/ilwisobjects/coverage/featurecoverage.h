#ifndef FEATURECOVERAGE_H
#define FEATURECOVERAGE_H

#include <memory>
#include <unordered_map>
#include "Kernel_global.h"

namespace Ilwis {

typedef std::vector<SPFeatureI> Features;

class FeatureIterator;
class FeatureFactory;

struct FeatureInfo {
    quint32 _count;
};

class KERNELSHARED_EXPORT FeatureCoverage : public Coverage
{
public:

    friend class FeatureIterator;

    FeatureCoverage();
    FeatureCoverage(const Resource &res);

    IlwisTypes featureTypes() const;
    void featureTypes(IlwisTypes types);
    SPFeatureI& newFeature(const Ilwis::Geometry &geom, quint32 itemId, const Ilwis::SPAttributeRecord &record);
    SPFeatureI newFeatureFrom(const Ilwis::SPFeatureI &existingFeature);
    quint32 featureCount(IlwisTypes types=itFEATURECOVERAGE, int index=iUNDEF) const;
    void setFeatureCount(IlwisTypes types, quint32 cnt);
    IlwisTypes ilwisType() const;

private:
    IlwisTypes _featureTypes;
    Features _features;
    std::vector<FeatureInfo> _featureInfo;
    FeatureFactory *_featureFactory;
    std::mutex _mutex2;


};

typedef IlwisData<FeatureCoverage> IFeatureCoverage;

}
Q_DECLARE_METATYPE(Ilwis::IFeatureCoverage)

#endif // FEATURECOVERAGE_H
