#ifndef FEATURE_H
#define FEATURE_H

#include "kernel_global.h"

namespace Ilwis {

class ColumnDefinition;
class UPFeatureI;
typedef std::unique_ptr<geos::geom::Geometry> UPGeometry;
class VertexIterator;

//typedef QSharedPointer<FeatureInterface> UPFeatureI;

class KERNELSHARED_EXPORT FeatureInterface {
public:
    virtual ~FeatureInterface() ;
    virtual quint64 featureid() const = 0;
    virtual bool isValid() const = 0 ;
    virtual const UPGeometry& geometry(const QVariant& trackIndex = QVariant()) const = 0;
    virtual UPGeometry& geometry(const QVariant& trackIndex = QVariant()) = 0;
    virtual void add(geos::geom::Geometry *geom,  const QVariant& trackIndex = QVariant()) = 0;
    virtual void remove(const QVariant& trackIndex) = 0;
    virtual void set(geos::geom::Geometry *geom,  const QVariant &trackIndexValue=QVariant()) = 0;
    //QVariant operator()(const QString& name, const QVariant &trackIndexValue=QVariant(COVERAGEATRIB), bool asRaw=true);
    virtual FeatureInterface *clone() const=0;
    virtual IlwisTypes geometryType(qint32 trackIndex=0) const = 0;
    virtual quint32 trackSize() const = 0;
    virtual QVariant cell(const QString& name, const QVariant &trackIndexValue=QVariant(COVERAGEATRIB), bool asRaw=true)  = 0;
    virtual void setCell(const QString& name, const QVariant& var, const QVariant &trackIndexValue=QVariant(COVERAGEATRIB)) = 0;
    virtual void setCell(quint32 colIndex, const QVariant& var, const QVariant &trackIndexValue=QVariant(COVERAGEATRIB)) = 0;
    virtual QVariant cell(quint32 colIndex, const QVariant &trackIndexValue=QVariant(COVERAGEATRIB), bool asRaw=true) = 0;
    virtual QVariant trackIndexValue(quint32 index = iUNDEF) const { return QVariant();}
    virtual ColumnDefinition columndefinition(const QString& name, bool coverages=true) const = 0;
    virtual ColumnDefinition columndefinition(quint32 index, bool coverages=true) const = 0;
    virtual quint32 attributeColumnCount(bool coverages=true) const = 0;
protected:


};

class KERNELSHARED_EXPORT UPFeatureI : public std::unique_ptr<FeatureInterface> {
public:
    UPFeatureI(FeatureInterface *f=0);
    QVariant operator ()(const QString &name, bool asRaw=true, int index = COVERAGEATRIB) const;
    void operator ()(int trackIndex, const QString &name, const QVariant& var);
    VertexIterator begin();
    VertexIterator end();
};

class Feature;

class KERNELSHARED_EXPORT GeometryNode : public FeatureInterface {

    friend class Feature;
public:
       ~GeometryNode() {}
private:
    GeometryNode();
    GeometryNode(geos::geom::Geometry *geom, Feature* feature, quint32 trackIndex );

    quint64 featureid() const ;
    bool isValid() const  ;
    const UPGeometry& geometry(const QVariant& trackIndex = QVariant()) const;
    UPGeometry& geometry(const QVariant& trackIndex = QVariant());
    void add(geos::geom::Geometry *,  const QVariant& trackIndex = QVariant() ) {}
    void remove(const QVariant& ) {}
    void set(geos::geom::Geometry *geom, const QVariant &trackIndexValue=QVariant());
    FeatureInterface *clone() const;
    IlwisTypes geometryType(qint32 trackIndex=0) const ;
    quint32 trackSize() const ;
    QVariant cell(quint32 colIndex, const QVariant &trackIndexValue=QVariant(COVERAGEATRIB), bool asRaw=true) ;
    virtual QVariant cell(const QString& name, const QVariant &trackIndexValue=QVariant(COVERAGEATRIB), bool asRaw=true) ;
    virtual void setCell(const QString& name, const QVariant& var, const QVariant &trackIndexValue=QVariant(COVERAGEATRIB));
    virtual void setCell(quint32 colIndex, const QVariant& var, const QVariant &trackIndexValue=QVariant(COVERAGEATRIB));
    ColumnDefinition columndefinition(const QString& name, bool coverages=true) const;
    ColumnDefinition columndefinition(quint32 index, bool coverages=true) const;
    quint32 attributeColumnCount(bool coverages=true) const;
    quint32 trackIndex() const;
    void setTrackIndex(quint32 ind);

    Feature *_feature;
    UPGeometry _geometry;
    quint32 _trackIndex;

};

class FeatureCoverage;
class AttributeRecord;

typedef std::unique_ptr<GeometryNode> UPFeatureNode;
typedef IlwisData<FeatureCoverage> IFeatureCoverage;
typedef std::unique_ptr<AttributeRecord> UPAttributeRecord;
typedef  std::pair<quint32, quint32> TrackIndexes;

/*!
The feature class represents a spatial object with a single identity and a one or more geometries. This is different from the regular
definition of a feature that has only one geometry. The reasoning for this is that an object that moves trough time is still the same
object. Same is true for objects at different scales. To be able to represent these kind of data structures a feature can have multiple
geometries organized in a vector. The index in the vector has meaning ( similar to z direction in gridcoverages) and uses the verticaldomain.
 */
class KERNELSHARED_EXPORT Feature : public FeatureInterface {
    friend class FeatureCoverage;
    friend class FeatureIterator;
    friend class GeometryNode;

public:
    Feature();
    Feature(IFeatureCoverage &fcoverage, int rec=iUNDEF) ;
    Feature(FeatureCoverage *fcoverage, int rec=iUNDEF);
    ~Feature();

    quint64 featureid() const;
    bool isValid() const ;
    const UPGeometry& geometry(const QVariant& trackIndexValue = QVariant()) const;
    UPGeometry& geometry(const QVariant& trackIndexValue = QVariant());
    void add(geos::geom::Geometry *geom,  const QVariant &trackIndexValue = QVariant());
    void remove(const QVariant& trackIndexValue);
    void set(geos::geom::Geometry *geom,  const QVariant &trackIndexValue = QVariant());
    FeatureInterface* clone() const;
    IlwisTypes geometryType(qint32 trackIndex=iUNDEF) const;
    quint32 trackSize() const;
    QVariant cell(quint32 colIndex, const QVariant &trackIndexValue=QVariant(COVERAGEATRIB), bool asRaw=true) ;
    QVariant cell(const QString& name, const QVariant &trackIndexValue=QVariant(COVERAGEATRIB), bool asRaw=true);
    void setCell(const QString& name, const QVariant& var, const QVariant &trackIndexValue=QVariant(COVERAGEATRIB));
    void setCell(quint32 colIndex, const QVariant& var, const QVariant &trackIndexValue=QVariant(COVERAGEATRIB)) ;
    ColumnDefinition columndefinition(const QString& name, bool coverages=true) const;
    ColumnDefinition columndefinition(quint32 index, bool coverages=true) const;
    quint32 attributeColumnCount(bool coverages=true) const;
    virtual QVariant trackIndexValue(quint32 index = iUNDEF) const;

private:
    Feature(const Feature& f) ; // nocopy constructor, _featureid is unique
    Feature(FeatureCoverage *fcoverage, AttributeRecord *rec);
    Feature& operator=(const Feature& f) ; // no assignment , _featureid is unique
    TrackIndexes getIndexes(const QVariant &trackIndexValue) const;

    static quint64 _idbase;
    quint64 _featureid; // unique
    std::vector<UPFeatureI> _track;
    UPAttributeRecord _record;
    FeatureCoverage *_parentFCoverage;
};



bool operator==(const Feature& f1, const Feature& f2) ;
}
Ilwis::FeatureInterface *createFeature(Ilwis::FeatureCoverage *fcoverage);
#endif // FEATURE_H
