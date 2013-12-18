#ifndef FEATURE_H
#define FEATURE_H

#include "Kernel_global.h"
#include "boost/optional.hpp"

namespace Ilwis {

class SPFeatureI;

//typedef QSharedPointer<FeatureInterface> SPFeatureI;

class KERNELSHARED_EXPORT FeatureInterface {
public:

    virtual ~FeatureInterface() ;

    /**
     * Query for the id of this feature
     * every feature is runtime unique and thus has a unique id
     *
     * @return the id of this feature
     */
    virtual quint64 featureid() const = 0;

    /**
     * checks if this feature is valid
     *
     * @return true when valid
     */
    virtual bool isValid() const = 0 ;

    /**
     * Returns a reference to the geometry of this feature at a certain index
     *
     * @param index the index of the geometry defaults to 0
     * @return the geometry of this feature at the given index
     */
    virtual const Geometry& geometry(quint32 index=0) const = 0;

    /**
     * Returns a reference to the geometry of this feature at a certain index
     *
     * @param index the index of the geometry defaults to 0
     * @return the geometry of this feature at the given index
     */
    virtual Geometry& geometry(quint32 index=0) = 0;

    /**
     * sets a Geometry on a given index of this feature
     *
     * @param geom the new geometry to be set
     * @param index the index at which the geometry should be set defaults to 0
     */
    virtual void set(const Geometry& geom, int index = 0) = 0;

    /**
     * This method gives acces to the attribute table of this feature, only 1 attribute table can be accessed based on the id of this feature
     *
     * @param name The name of the attribute
     * @param index The index of the attribute
     * @param asRaw when set to true will return only the raw value (defaults to true because setting it to false will cause efficiency loss)
     * @return the requested attribute
     */
    QVariant operator()(const QString& name, int index = -1, bool asRaw=true);

    /**
     * Clones / copies this feature
     * @return a Copy of this feature
     */
    virtual FeatureInterface *clone() const=0;

    /**
     * Query for the geometry type of this feature
     * @param index
     * @return
     */
    virtual IlwisTypes geometryType(qint32 index=0) const = 0;

    /**
     * @brief trackSize
     * @return
     */
    virtual quint32 trackSize() const = 0;

    /**
     * @brief cell
     * @param name
     * @param index
     * @param asRaw
     * @return
     */
    virtual QVariant cell(const QString& name, int index=-1, bool asRaw=true)  = 0;

    /**
     * @brief setCell
     * @param name
     * @param var
     * @param index
     */
    virtual void setCell(const QString& name, const QVariant& var, int index=-1) = 0;

    /**
     * @brief setCell
     * @param colIndex
     * @param var
     * @param index
     */
    virtual void setCell(quint32 colIndex, const QVariant& var, int index=-1) = 0;

    /**
     * @brief cell
     * @param colIndex
     * @param index
     * @param asRaw
     * @return
     */
    virtual QVariant cell(quint32 colIndex, int index=-1, bool asRaw=true) = 0;

    /**
     * @brief columndefinition
     * @param name
     * @param coverages
     * @return
     */
    virtual ColumnDefinition columndefinition(const QString& name, bool coverages=true) const = 0;

    /**
     * @brief columndefinition
     * @param index
     * @param coverages
     * @return
     */
    virtual ColumnDefinition columndefinition(quint32 index, bool coverages=true) const = 0;

    /**
     * @brief attributeColumnCount
     * @param coverages
     * @return
     */
    virtual quint32 attributeColumnCount(bool coverages=true) const = 0;
protected:


};

class KERNELSHARED_EXPORT SPFeatureI : public std::unique_ptr<FeatureInterface> {
public:
    SPFeatureI(FeatureInterface *f=0);
    QVariant operator ()(const QString &name, int index=-1, bool asRaw=true);
    void operator ()(const QString &name, const QVariant& var, int index=-1);
};

class Feature;

class KERNELSHARED_EXPORT FeatureNode : public FeatureInterface {

    friend class Feature;
public:
       ~FeatureNode() {}
private:
    FeatureNode();
    FeatureNode(const Ilwis::Geometry &geometry, Feature* feature, quint32 index );

    virtual quint64 featureid() const ;
    virtual bool isValid() const  ;
    virtual const Ilwis::Geometry &geometry(quint32 index=0) const;
    virtual Ilwis::Geometry &geometry(quint32 index=0);
    virtual void set(const Geometry& geom, int index = 0) ;
    virtual FeatureInterface *clone() const;
    virtual IlwisTypes geometryType(qint32 index=0) const ;
    virtual quint32 trackSize() const ;
    QVariant cell(quint32 colIndex, int index=-1, bool asRaw=true) ;
    virtual QVariant cell(const QString& name, int index=-1, bool asRaw=true) ;
    virtual void setCell(const QString& name, const QVariant& var, int index=-1);
    virtual void setCell(quint32 colIndex, const QVariant& var, int index=-1);
    ColumnDefinition columndefinition(const QString& name, bool coverages=true) const;
    ColumnDefinition columndefinition(quint32 index, bool coverages=true) const;
    quint32 attributeColumnCount(bool coverages=true) const;
    quint32 index() const;
    void setIndex(quint32 ind);

    Feature *_feature;
    Geometry _geometry;
    quint32 _index;

};

class FeatureCoverage;

typedef std::shared_ptr<FeatureNode> SPFeatureNode;
typedef IlwisData<FeatureCoverage> IFeatureCoverage;

/*!
The feature class represents a spatial object with a single identity and a one or more geometries. This is different from the regular
definition of a feature that has only one geometry. The reasoning for this is that an object that moves trough time is still the same
object. Same is true for objects at different scales. To be able to represent these kind of data structures a feature can have multiple
geometries organized in a vector. The index in the vector has meaning ( similar to z direction in gridcoverages) and uses the verticaldomain.
 */
class KERNELSHARED_EXPORT Feature : public FeatureInterface {
    friend class FeatureCoverage;
    friend class FeatureIterator;
    friend class FeatureNode;

public:
    Feature();
    Feature(const Ilwis::IFeatureCoverage &fcoverage, int rec=iUNDEF) ;
    Feature(const FeatureCoverage* fcoverage, int rec=iUNDEF);
    ~Feature();

    quint64 featureid() const;
    bool isValid() const ;
    Geometry& geometry(quint32 index=0);
    const Geometry& geometry(quint32 index=0) const;
    void set(const Geometry& geom, int index = 0);
    FeatureInterface* clone() const;
    IlwisTypes geometryType(qint32 index=iUNDEF) const;
    quint32 trackSize() const;
    QVariant cell(quint32 colIndex, int index=-1, bool asRaw=true) ;
    QVariant cell(const QString& name, int index=-1, bool asRaw=true);
    void setCell(const QString& name, const QVariant& var, int index=-1);
    void setCell(quint32 colIndex, const QVariant& var, int index=-1) ;
    ColumnDefinition columndefinition(const QString& name, bool coverages=true) const;
    ColumnDefinition columndefinition(quint32 index, bool coverages=true) const;
    quint32 attributeColumnCount(bool coverages=true) const;

private:
    Feature(const Feature& f) ; // nocopy constructor, _featureid is unique
    Feature(AttributeRecord *rec);
    Feature& operator=(const Feature& f) ; // no assignment , _featureid is unique

    static quint64 _idbase;
    quint64 _featureid; // unique
    std::vector<SPFeatureNode> _track;
    SPAttributeRecord _record;
    Geometry _invalidGeom;

};



bool operator==(const Feature& f1, const Feature& f2) ;

}
Ilwis::FeatureInterface *createFeature(Ilwis::FeatureCoverage *fcoverage);
#endif // FEATURE_H
