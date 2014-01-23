#ifndef FEATURE_H
#define FEATURE_H

#include "Kernel_global.h"
#include "boost/optional.hpp"

namespace Ilwis {

class SPFeatureI;

//typedef QSharedPointer<FeatureInterface> SPFeatureI;

/**
 * Interface for a standard feature class,
 */
class KERNELSHARED_EXPORT FeatureInterface {
public:

    virtual ~FeatureInterface() ;

    /**
     * Query for the id of this feature<br>
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
     * Query for the geometry type of this feature<br>
     *
     * can return:
     * - //welke types
     * -
     * @param index the index at which you want to know the geometry type
     * \sa IlwisObject
     * @return the ilwistype of the geometry at the given index
     */
    //TODO fill in the possible return types
    virtual IlwisTypes geometryType(qint32 index=0) const = 0;

    /**
     * Query for the track size of this feature<br>
     * the track size equals the amount of indices this feature has
     *
     * @return an integer equal to the amount of indices, will not be smaller than 0
     */
    virtual quint32 trackSize() const = 0;

    /**
     * Queries the attribute table record of this feature ( a feature can only have 1 record)
     * for the given cell.<br>
     *
     * Disabling asRaw will decrease performance
     *
     * @param name the name of the collumn
     * @param index the index of the cell, usually refers to a point in time, defaults to -1 (standard index)
     * @param asRaw when set to true will return the raw value, defaults to true
     * @return The contents of the cell
     */
    virtual QVariant cell(const QString& name, int index=-1, bool asRaw=true)  = 0;

    /**
     * Sets a new value in a specified cell from the attributetable record of this feature
     *
     * @param name the collumn name of the destination cell
     * @param var the new value of the destination cell
     * @param index index of the destination cell, when relevant, defaults to -1 (standard index)
     */
    virtual void setCell(const QString& name, const QVariant& var, int index=-1) = 0;

    /**
     * Sets a new value in a specified cell from the attributetable record of this feature
     *
     * @param colIndex index of the collumn of the destination cell
     * @param var the new value of the destination cell
     * @param index index of the destination cell, when relevant, defaults to -1 (standard index)
     */
    virtual void setCell(quint32 colIndex, const QVariant& var, int index=-1) = 0;

    /**
     * Queries the attribute table record of this feature ( a feature can only have 1 record)
     * for the given cell.<br>
     *
     * Disabling asRaw will decrease performance
     *
     * @param colIndex index of the collumn of the cell
     * @param index the index of the cell, usually refers to a point in time, defaults to -1 (standard index)
     * @param asRaw when set to true will return the raw value, defaults to true
     * @return The contents of the cell
     */
    virtual QVariant cell(quint32 colIndex, int index=-1, bool asRaw=true) = 0;

    /**
     * Query for the ColumnDefinition, returns the ColumnDefinition of the index or the coverage table
     *
     * @param name name of the collumn you want the ColumnDefinition from
     * @param coverages set to false if you want to know the collumns in the index table
     * @return The ColumnDefinition of the specified table of the collumn with the specified name
     * \sa ColumnDefinition
     */
    virtual ColumnDefinition columndefinition(const QString& name, bool coverages=true) const = 0;

    /**
     * Query for the collumndefinition, returns the Columndefinition from the record this feature has in the attribute table
     *
     * @param index the index of the collumen you want the definition of
     * @return The ColumnDefinition of the specified column
     * \sa ColumnDefinition
     */
    virtual ColumnDefinition columndefinition(quint32 index, bool coverages=true) const = 0;

    /**
     * Query for the amount of collumns in the index or coverage table.
     *
     * @param coverages set to false if you want to know the collumns in the index table
     * @return the amount of collumns in the specified table
     */
    //TODO link to att /index table?
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

    /**
     * Constructor for an empty FeatureNode
     */
    FeatureNode();

    /**
     * Constructor for a FeatureNode
     *
     * @param geometry the geometry of the new FeatureNode
     * @param feature the feature of the new FeatureNode
     * @param index the index of the new FeatureNode
     */
    FeatureNode(const Ilwis::Geometry &geometry, Feature* feature, quint32 index );

    //@override
    virtual quint64 featureid() const ;

    /**
     * Checks if this FeatureNode is valid<br>
     * A FeatureNode is always valid
     * @return true, always
     */
    virtual bool isValid() const  ;

    //@override
    virtual const Ilwis::Geometry &geometry(quint32 index=0) const;

    //@override
    virtual Ilwis::Geometry &geometry(quint32 index=0);

    //@override
    virtual void set(const Geometry& geom, int index = 0) ;

    //@override
    virtual FeatureInterface *clone() const;

    //@override
    virtual IlwisTypes geometryType(qint32 index=0) const ;

    //@override
    virtual quint32 trackSize() const ;

    //@override
    QVariant cell(quint32 colIndex, int index=-1, bool asRaw=true) ;

    //@override
    virtual QVariant cell(const QString& name, int index=-1, bool asRaw=true) ;

    //@override
    virtual void setCell(const QString& name, const QVariant& var, int index=-1);

    //@override
    virtual void setCell(quint32 colIndex, const QVariant& var, int index=-1);

    //@override
    ColumnDefinition columndefinition(const QString& name, bool coverages=true) const;

    //@override
    ColumnDefinition columndefinition(quint32 index, bool coverages=true) const;

    //@override
    quint32 attributeColumnCount(bool coverages=true) const;

    /**
     * Query for the index of this FeatureNode
     * @return the index of this FeatureNode
     */
    quint32 index() const;

    /**
     * Sets a new index for this FeatureNode
     * @param ind the new index
     */
    void setIndex(quint32 ind);

    //TODO private?
    Feature *_feature;
    Geometry _geometry;
    quint32 _index;

};

class FeatureCoverage;

typedef std::shared_ptr<FeatureNode> SPFeatureNode;
typedef IlwisData<FeatureCoverage> IFeatureCoverage;

/*!
 * The feature class represents a spatial object with a single identity and a one or more geometries. This is different from the regular <br>
 * definition of a feature that has only one geometry. The reasoning for this is that an object that moves trough time is still the same <br>
 * object. Same is true for objects at different scales. To be able to represent these kind of data structures a feature can have multiple <br>
 * geometries organized in a vector. The index in the vector has meaning ( similar to z direction in gridcoverages) and uses the verticaldomain. <br>
 */
class KERNELSHARED_EXPORT Feature : public FeatureInterface {
    friend class FeatureCoverage;
    friend class FeatureIterator;
    friend class FeatureNode;

public:
    /**
     * constructs an empty Feature
     */
    Feature();

    /**
     * Constructs a new feature from a IFeatureCoverage
     *
     * @param fcoverage the coverage that should be used to construct this feature
     * @param rec amount of records in the attribute table
     */
    Feature(const Ilwis::IFeatureCoverage &fcoverage, int rec=iUNDEF) ;

    /**
     * Constructs a new feature from a FeatureCoverage*
     *
     * @param fcoverage the feature coverage this feature should be constructed from
     * @param rec amount of entries in the attributetable
     */
    Feature(const FeatureCoverage* fcoverage, int rec=iUNDEF);

    ~Feature();

    //@override
    quint64 featureid() const;

    /**
     * Checks if this feature is valid<br>
     *
     * A Feature is valid when its record is valid
     *
     * @return true if this feature is valid
     */
    bool isValid() const ;

    //@override
    Geometry& geometry(quint32 index=0);

    //@override
    const Geometry& geometry(quint32 index=0) const;

    //@override
    void set(const Geometry& geom, int index = 0);

    //@override
    FeatureInterface* clone() const;

    //@override
    IlwisTypes geometryType(qint32 index=iUNDEF) const;

    //@override
    quint32 trackSize() const;

    //@override
    QVariant cell(quint32 colIndex, int index=-1, bool asRaw=true) ;

    //@override
    QVariant cell(const QString& name, int index=-1, bool asRaw=true);

    //@override
    void setCell(const QString& name, const QVariant& var, int index=-1);

    //@override
    void setCell(quint32 colIndex, const QVariant& var, int index=-1) ;

    //@override
    ColumnDefinition columndefinition(const QString& name, bool coverages=true) const;

    //@override
    ColumnDefinition columndefinition(quint32 index, bool coverages=true) const;

    //@override
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
