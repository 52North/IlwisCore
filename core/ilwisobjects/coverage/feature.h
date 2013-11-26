#ifndef FEATURE_H
#define FEATURE_H

#include "Kernel_global.h"

namespace Ilwis {

class SPFeatureI;

//typedef QSharedPointer<FeatureInterface> SPFeatureI;

class KERNELSHARED_EXPORT FeatureInterface {
public:
    virtual ~FeatureInterface() {}
    virtual quint64 featureid() const = 0;
    virtual bool isValid() const = 0 ;
    virtual const Geometry& geometry(quint32 index=0) const = 0;
    virtual void set(const Geometry& geom, int index = 0) = 0;
    QVariant operator()(const QString& name, int index = -1, bool asRaw=true);
    virtual FeatureInterface *clone() const=0;
    virtual IlwisTypes ilwisType(qint32 index=0) const = 0;
    virtual quint32 trackSize() const = 0;
    virtual QVariant cell(const QString& name, int index=-1, bool asRaw=true) = 0;
    virtual void setCell(const QString& name, const QVariant& var, int index=-1) = 0;
    virtual ColumnDefinition columndefinition(const QString& name, bool coverages=true) const = 0;
protected:


};

class KERNELSHARED_EXPORT SPFeatureI : public QSharedPointer<FeatureInterface> {
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
    virtual const Geometry& geometry(quint32 index=0) const ;
    virtual void set(const Geometry& geom, int index = 0) ;
    virtual FeatureInterface *clone() const;
    virtual IlwisTypes ilwisType(qint32 index=0) const ;
    virtual quint32 trackSize() const ;
    virtual QVariant cell(const QString& name, int index=-1, bool asRaw=true) ;
    virtual void setCell(const QString& name, const QVariant& var, int index=-1);
    ColumnDefinition columndefinition(const QString& name, bool coverages=true) const;
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
    Feature(const Ilwis::IFeatureCoverage &fcoverage) ;
    Feature(const FeatureCoverage* fcoverage);
    ~Feature();

    quint64 featureid() const;
    bool isValid() const ;
    const Geometry& geometry(quint32 index=0) const;
    void set(const Geometry& geom, int index = 0);
    FeatureInterface* clone() const;
    IlwisTypes ilwisType(qint32 index=iUNDEF) const;
    quint32 trackSize() const;
    QVariant cell(const QString& name, int index=-1, bool asRaw=true);
    void setCell(const QString& name, const QVariant& var, int index=-1);
    ColumnDefinition columndefinition(const QString& name, bool coverages=true) const;

private:
    Feature(const Feature& f) ; // nocopy constructor, _featureid is unique
    Feature(const SPAttributeRecord& rec);
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
