#ifndef ILWISINTERFACES_H
#define ILWISINTERFACES_H

namespace geos{
namespace geom{
class Geometry;
class GeometryFactory;
}
}
namespace Ilwis {

class FeatureInterface;
class SPFeatureI;
class Record;
class FeatureCoverage;
class ColumnDefinition;

typedef IlwisData<FeatureCoverage> IFeatureCoverage;
typedef std::unique_ptr<geos::geom::Geometry> UPGeometry;
typedef std::unique_ptr<geos::geom::GeometryFactory> UPGeomFactory;

class KERNELSHARED_EXPORT FeatureVariantInterface {
public:
    virtual SPFeatureI subFeatureRef(double variantItem)  = 0;
    virtual SPFeatureI subFeatureRef(const QString &variantItem)  = 0;
    virtual void removeSubFeature(const QString& variantItem)  = 0;
    virtual void setSubFeature(const QString &variantItem, FeatureInterface *feature)  = 0;
    virtual void setSubFeature(const QString &variantItem, SPFeatureI& feature) = 0;
    virtual void removeSubFeature(double variantItem)  = 0;
    virtual void setSubFeature(double variantItem, FeatureInterface *feature)  = 0;
    virtual void setSubFeature(double variantItem, SPFeatureI& feature) = 0;
    virtual quint32 subFeatureCount() const = 0;
};

class KERNELSHARED_EXPORT CellInterface {
public:
    virtual QVariant cell(quint32 colIndex, bool asRaw=true) const  = 0;
    virtual QVariant cell(const QString& columnname, bool asRaw=true) const  = 0;
    virtual void setCell(const QString& columnName, const QVariant& cellValue)  = 0;
    virtual void setCell(quint32 colIndex, const QVariant& cellValue) = 0 ;
};

class KERNELSHARED_EXPORT FeatureInterface : public FeatureVariantInterface, public CellInterface{
    friend class FeatureCoverage;
    friend class FeatureIterator;

public:
    virtual quint64 featureid() const = 0;
    virtual bool isValid() const = 0 ;

    virtual FeatureInterface* clone(FeatureCoverage *fcoverage) const = 0;
    virtual SPFeatureI createSubFeature(const QString& variantItem, geos::geom::Geometry *geom) = 0;

    virtual IlwisTypes geometryType() const  = 0;
    virtual void geometry(geos::geom::Geometry *geom)  = 0;
    virtual const UPGeometry& geometry() const = 0;
    //virtual UPGeometry& geometryRef() = 0;

    virtual Record& recordRef() = 0;
    virtual const Record& record() const = 0;
    virtual void record(const std::vector<QVariant> &values, quint32 offset=0) = 0;

    virtual ColumnDefinition attributedefinition(const QString& name) const  = 0;
    virtual ColumnDefinition attributedefinition(quint32 columnIndex) const  = 0;
    virtual quint32 attributeColumnCount() const  = 0;

    virtual void store(const FeatureAttributeDefinition &columns, QDataStream &stream, const IOOptions &options) = 0;
    virtual void load(const FeatureAttributeDefinition& columns, QDataStream& stream, const IOOptions &options) = 0;

};



class KERNELSHARED_EXPORT FeatureCoverageInterface{
public:
    virtual ~ FeatureCoverageInterface() {}
    virtual SPFeatureI newFeature(geos::geom::Geometry *geom, bool load=true) = 0;
    virtual SPFeatureI newFeature(const QString& wkt, const Ilwis::ICoordinateSystem &csy=ICoordinateSystem(), bool load=true) = 0;
    virtual SPFeatureI newFeatureFrom(const Ilwis::SPFeatureI &existingFeature, const Ilwis::ICoordinateSystem &csySource=ICoordinateSystem())  = 0;

    virtual IlwisTypes featureTypes() const  = 0;
    virtual void featureTypes(IlwisTypes types) = 0;
    virtual quint32 featureCount(IlwisTypes types, quint32 level) const  = 0;
    virtual void setFeatureCount(IlwisTypes types, qint32 featureCnt,quint32 level)  = 0;

    virtual ITable attributeTable(quint32 level)  = 0;
    virtual void attributesFromTable(const ITable &otherTable) = 0;
    virtual FeatureAttributeDefinition& attributeDefinitionsRef(qint32 level=0)  = 0 ;
    virtual const FeatureAttributeDefinition& attributeDefinitions(qint32 level = 0) const  = 0;

    virtual const UPGeomFactory &geomfactory() const  = 0;
    virtual std::vector<quint32> select(const QString& spatialQuery) const  = 0;
};
}
#endif // ILWISINTERFACES_H
