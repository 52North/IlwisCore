#ifndef POSTGRESQLFEATURECOVERAGELOADER_H
#define POSTGRESQLFEATURECOVERAGELOADER_H

namespace Ilwis {

namespace Postgresql {

struct MetaGeometryColumn;

class PostgresqlFeatureCoverageLoader
{
public:
    PostgresqlFeatureCoverageLoader(const Resource &resource, const IOOptions &options);
    ~PostgresqlFeatureCoverageLoader();

    bool loadMetadata(FeatureCoverage *fcoverage) const;
    bool loadData(FeatureCoverage *fcoverage) const;
    bool storeData(FeatureCoverage *fcoverage) const;

private:
    Resource _resource;
    IOOptions _options;

    void setFeatureCount(FeatureCoverage *fcoverage) const;
    void setSpatialMetadata(FeatureCoverage *fcoverage) const;
    void setSubfeatureSemantics(Ilwis::FeatureCoverage *fcoverage, Ilwis::IDomain &semantics) const;

    QString selectGeometries(const QList<MetaGeometryColumn> &metaGeometry) const;
    geos::geom::Geometry* createGeometry(QSqlQuery &query, QString geomColumn, ICoordinateSystem crs) const;
};


}
}

#endif // POSTGRESQLFEATURECOVERAGELOADER_H
