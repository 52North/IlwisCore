#ifndef SPATIALRELATION_H
#define SPATIALRELATION_H

namespace Ilwis {
namespace BaseOperations {

typedef std::function<bool(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2)> SpatialRelation;


class SpatialRelationOperation : public OperationImplementation
{
public:
    SpatialRelationOperation();

    SpatialRelationOperation(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable &symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid, const Ilwis::OperationExpression &expr);
protected:
    static quint64 createMetadata();
    OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    IFeatureCoverage _inputFeatures;
    IFeatureCoverage _relatedFeatures;
    IFeatureCoverage _outputFeatures;
    std::unique_ptr<geos::geom::Geometry> _geometry;
    SpatialRelation _relation;
private:
    void collectResults(const geos::geom::Geometry *geomRelation, SPFeatureI inputfeature, std::vector<SPFeatureI>& outFeatures);
    static bool disjoint(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;
    static bool contains(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;
    static bool covers(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;
    static bool coveredBy(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;
    static bool touches(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;
    static bool intersects(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;
    static bool within(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;
    static bool crosses(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;
    static bool overlaps(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;
    static bool equals(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;

    NEW_OPERATION(SpatialRelationOperation);
};
}
}

#endif // SPATIALRELATION_H
