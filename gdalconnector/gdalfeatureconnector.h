#ifndef GDALFEATURECONNECTOR_H
#define GDALFEATURECONNECTOR_H

namespace Ilwis{
namespace Gdal {

struct SourceHandles{
    SourceHandles(OGRDataSourceH source=0, const std::vector<OGRLayerH>& layer=std::vector<OGRLayerH>()) : _source(source), _layers(layer) {}
    OGRDataSourceH _source;
    std::vector<OGRLayerH> _layers;
};
typedef std::vector<SourceHandles> FeatureSetHandles;

class GdalFeatureConnector : public CoverageConnector{
public:
    GdalFeatureConnector(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());

    bool loadMetaData(IlwisObject* data, const IOOptions &options);
    bool loadData(IlwisObject* data, const Ilwis::IOOptions &options);
    bool store(IlwisObject *obj,const IOOptions& options = IOOptions());

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    Ilwis::IlwisObject *create() const;
protected:

private:
    OGRSFDriverH _driver;

    enum OutputState { osIndex2Layer=1,osType2Layer=2, osLayer2DataSource=4};

    QVariant fillEmptyColumn(OGRFeatureH, int);

    geos::geom::Geometry* fillFeature(FeatureCoverage *fcoverage, OGRGeometryH geometry) const;
    geos::geom::Geometry* fillPoint(FeatureCoverage *fcoverage, OGRGeometryH geometry) const;
    geos::geom::Geometry* fillLine(FeatureCoverage *fcoverage, OGRGeometryH geometry) const;
    geos::geom::Geometry* fillPolygon(FeatureCoverage *fcoverage, OGRGeometryH geometry) const;
    geos::geom::Geometry* fillMultiPoint(FeatureCoverage *fcoverage, OGRGeometryH geometry) const;
    geos::geom::Geometry* fillMultiLine(FeatureCoverage *fcoverage, OGRGeometryH geometry) const;
    geos::geom::Geometry* fillMultiPolygon(FeatureCoverage *fcoverage, OGRGeometryH geometry) const;
    geos::geom::Geometry* fillGeometryCollection(FeatureCoverage *fcoverage, OGRGeometryH geometry) const;
    OGRDataSourceH createFileBasedDataSource(const QString &postfix, const QFileInfo &fileinfo) const;
    OGRLayerH createLayer(const QString &name, OGRwkbGeometryType type, OGRSpatialReferenceH srs, OGRDataSourceH source);
    bool createAttributes(const ITable &tbl, OGRLayerH layer, const std::vector<OGRFieldDefnH> &fielddefs,std::vector<bool>& validAttributes);
    bool loadDriver();
    OGRwkbGeometryType ilwisType2GdalFeatureType(IlwisTypes tp);
    void setAttributes(OGRFeatureH hfeature, Ilwis::SPFeatureI& feature, const std::vector<bool> &validAttributes, const std::vector<Ilwis::ColumnDefinition> &def);
    bool setDataSourceAndLayers(const IFeatureCoverage &features, std::vector<SourceHandles> &datasources,std::vector<bool>& validAttributes);
    OGRGeometryH createFeature(const geos::geom::Geometry* geom);
    OGRGeometryH createLine(const geos::geom::Geometry* geom);
    OGRGeometryH createPoint(const geos::geom::Geometry* geom);
    OGRGeometryH createPolygon(const geos::geom::Geometry* geom);
    OGRGeometryH createMultiLine(const geos::geom::Geometry* geom);
    OGRGeometryH createMultiPoint(const geos::geom::Geometry* geom);
    OGRGeometryH createMultiPolygon(const geos::geom::Geometry* geom);
    OGRGeometryH createGeometryCollection(const geos::geom::Geometry* geom);

    static int ilwisType2Index(IlwisTypes);
    bool createDataSourceAndLayers(IlwisTypes types,
                                   const QString &postfix,
                                   const IFeatureCoverage &features,
                                   OGRSpatialReferenceH srs,
                                   const std::vector<OGRFieldDefnH> &fielddefs,
                                   std::vector<SourceHandles> &datasources,
                                   std::vector<bool> &validAttributes);
};
}
}
#endif // GDALFEATURECONNECTOR_H


