#ifndef FEATURECOVERAGE_H
#define FEATURECOVERAGE_H

#include <memory>
#include <unordered_map>
#include "Kernel_global.h"

namespace Ilwis {

class FeatureNode;

typedef std::vector<SPFeatureI> Features;

class FeatureIterator;
class FeatureFactory;
class AttributeRecord;

struct FeatureInfo {
    quint32 _count;
};

class KERNELSHARED_EXPORT FeatureCoverage : public Coverage
{
public:

    friend class FeatureIterator;

    FeatureCoverage();
    FeatureCoverage(const Resource &resource);

    IlwisTypes featureTypes() const;
    void featureTypes(IlwisTypes types);
    SPFeatureI &newFeature(const Ilwis::Geometry &geom);
    SPFeatureI newFeatureFrom(const Ilwis::SPFeatureI &existingFeature, const Ilwis::ICoordinateSystem &csySource=ICoordinateSystem());
    quint32 featureCount(IlwisTypes types=itFEATURE, int index=iUNDEF) const;
    void setFeatureCount(IlwisTypes types, quint32 cnt);
    IlwisTypes ilwisType() const;
    FeatureCoverage *copy();
    QSharedPointer<AttributeRecord> record() const;

protected:
    void copyTo(IlwisObject *obj);
private:
    IlwisTypes _featureTypes;
    Features _features;
    std::vector<FeatureInfo> _featureInfo;
    FeatureFactory *_featureFactory;
    std::mutex _mutex2;
    QSharedPointer<AttributeRecord> _record;


};

typedef IlwisData<FeatureCoverage> IFeatureCoverage;

}
Q_DECLARE_METATYPE(Ilwis::IFeatureCoverage)

#endif // FEATURECOVERAGE_H
