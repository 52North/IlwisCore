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
    std::vector<quint32> _perIndex;
};
/**
 * A feature in Ilwis-Objects is anything that can have one or more geometries and has one distinct
 * identity. In the Ilwis-Objects terminology an ice berg tracked over time (assuming it doesn’t split) can
 * be represented as one feature. It may change shape and/or location in time but it is still the same ice
 * berg. The track of geometries is referred to as the ‘index ‘, similar to the same notion in the raster
 * data model.
 * A feature at a certain index is referred to as feature-element and is similar to a traditional elements
 * of vector maps.
 * Apart from geometries, features also have extensible set of other properties (the attributes) that
 * depend on the nature of the feature and context in which it is used.
 * Organization
 * On a programmatic level sets of features are organized in a feature coverage. Feature coverages are
 * collections of features that share one or more properties (e.g. within the same bounding box).
 *
 * Features are thus described by
 * - One or more spatial locations.
 * + Per index entry a geometry ( which maybe an empty geometry)
 * + Index dependent attributes
 * - Index independent attributes.
 *
 * The consequence is that feature-coverages are described by
 * - A set of features (duh)
 * - A common spatial reference system else the spatial location would not make sense
 * - common index-domain or else the index values make no sense. Usually a value (related) domain like value, time or scale but others are also possible.
 * - A common attribute table.
 *
 * Indexes are mapped onto a domain. A few notes here:
 *
 * 1. The traditional feature layer representation is of course still possible. It is just a feature set
 * with only one index and one geometry type.
 * 2. A feature set is not necessarily mono-type, not even when they are restricted to one index.
 * Features happily coexist with different geometries. This doesn’t prevent mono-type sets
 * though.
 * 3. As with rasters, movement through the feature set is controlled by an iterator. This iterator
 * can (optionally) discriminate between the various geometry types(and other properties).
 * Every feature is tied to a record in an attribute table through its featureid. A featureid is a run-time
 * unique id that is generated for every feature during the run time existence of this instance of Ilwis-
 * Objects. No meaning or persistence is attached to the featureid. It is only a glue between feature-set
 * and attribute table.
 *
 * The index may have an attribute table were attributes are stored that are solely dependent on the
 * featureid + index key combination.
 *
 * \sa FeatureIterator
 */
class KERNELSHARED_EXPORT FeatureCoverage : public Coverage
{
public:

    friend class FeatureIterator;

    /**
     * Constructor for an empty FeatureCoverage
     */
    FeatureCoverage();
    ~FeatureCoverage();

    /**
     * Constructs a FeatureCoverage from a Resource
     *
     * \sa Resource
     * @param resource the resource to be used
     */
    FeatureCoverage(const Resource &resource);

    //@override
    IlwisTypes featureTypes() const;

    /**
     * Set method for the featureTypes, changes the feature type field to the given value
     *
     * @param types the new feature type
     */
    void featureTypes(IlwisTypes types);

    /**
     * Creates a new SPFeatureI from the given geometry
     * you could say the geometry gets transformed into a feature
     *
     * \sa SPFeatureI
     * \sa Geometry
     * @param geom the geometry that has to be used for the new feature
     * @return returns the new feature, can be a nullptr if the geometry was invalid
     */
    SPFeatureI newFeature(const Ilwis::Geometry &geom);

    /**
     * Creates a new Feature from an existing Feature and a coordinatesystem
     * if no coordinate system specified the coordinate system of this coverage will be used
     *
     * A new feature will always get a attribute record upon creation
     *
     * you could say this generates a copy of the existing feature with a new id and such
     *
     * \sa SPFeatureI
     * \sa ICoordinateSystem
     * @param existingFeature the Feature that should be used as a template for the new one
     * @param csySource the coordinate system that should be used must of course be compatible with the given Feature
     * @return A new feature or a nullptr if the given Feature was invalid
     */
    SPFeatureI newFeatureFrom(const Ilwis::SPFeatureI &existingFeature, const Ilwis::ICoordinateSystem &csySource=ICoordinateSystem());

    /**
     * Counts the amount of features of a given type in this FeatureCoverage, if you use the default value all features will be counted.
     * the index is used the specify where in the third dimension of this coverage we should count, you only need to specify this if this coverage
     * actually has a third dimension.
     *
     * @param types the type of feature that should be counted
     * @param index specification of the third dimension, when required
     * @return the amount of features of this type
     */
    quint32 featureCount(IlwisTypes types=itFEATURE, int index=iUNDEF) const;

    /**
     * Changes the amount of features for a certain featuretype
     * note if you use this function multiple times with different types, it will change the count of every type that has been changed before
     * so:
     * setFeatureCount(type1,3);
     * setFeatureCount(type2,4);
     * sets the amount of both type 1 and type 2 to 4
     *
     * @param types the type that should be set
     * @param cnt the count that should be set
     */
    void setFeatureCount(IlwisTypes types, quint32 cnt, int index=0);
    quint32 maxIndex() const;

    //@override
    IlwisTypes ilwisType() const;

    /**
     * Copies all fields onto a new FeatureCoverage
     *
     * @return a New FeatureCoverage with the same values as this
     */
    FeatureCoverage *clone();

protected:
    void copyTo(IlwisObject *obj);
private:
    IlwisTypes _featureTypes;
    Features _features;
    std::vector<FeatureInfo> _featureInfo;
    FeatureFactory *_featureFactory;
    std::mutex _mutex2;
    quint32 _maxIndex;


    Ilwis::SPFeatureI createNewFeature(IlwisTypes tp);
    void adaptFeatureCounts(int tp, quint32 cnt, int index);
};

typedef IlwisData<FeatureCoverage> IFeatureCoverage;

}
Q_DECLARE_METATYPE(Ilwis::IFeatureCoverage)

#endif // FEATURECOVERAGE_H
