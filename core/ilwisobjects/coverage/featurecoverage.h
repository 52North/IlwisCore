#ifndef FEATURECOVERAGE_H
#define FEATURECOVERAGE_H

#include <memory>
#include <unordered_map>
#include "Kernel_global.h"

namespace Ilwis {

typedef std::vector<SPFeature> Features;

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
    SPFeature& newFeature(const Ilwis::Geometry &geom, quint64 itemId, const Ilwis::SPAttributeRecord &record);
    quint32 featureCount(IlwisTypes types=itFEATURECOVERAGE) const;
    void setFeatureCount(IlwisTypes types, quint32 cnt);
    IlwisTypes ilwisType() const;

private:
    IlwisTypes _featureTypes;
    Features _features;
    std::vector<FeatureInfo> _featureInfo;
    FeatureFactory *_featureFactory;


};

//typedef IlwisData<FeatureCoverage> IFeatureCoverage;

class IFeatureCoverage : public IlwisData<FeatureCoverage> {
public:
    QVariant operator()(quint32 fnumber, const QString& column, const QString& indexvalue="");
};

}

Q_DECLARE_METATYPE(Ilwis::IFeatureCoverage)

#endif // FEATURECOVERAGE_H
