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
protected:
    static quint64 createMetadata(Ilwis::OperationResource &operation);
    OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    ICoverage _coverage;
    IFeatureCoverage _outputFeatures;
    std::unique_ptr<geos::geom::Geometry> _geometry;
    SpatialRelation _relation;
};

class Contains : public SpatialRelationOperation
{
public:
    Contains();

    Contains(quint64 metaid, const Ilwis::OperationExpression &expr);

    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

   NEW_OPERATION(Contains);

protected:
   static bool contains(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;

};

class Covers : public SpatialRelationOperation
{
public:
    Covers();

    Covers(quint64 metaid, const Ilwis::OperationExpression &expr);

    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

   NEW_OPERATION(Covers);

protected:
   static bool covers(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;

};

class CoveredBy : public SpatialRelationOperation
{
public:
    CoveredBy();

    CoveredBy(quint64 metaid, const Ilwis::OperationExpression &expr);

    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

   NEW_OPERATION(CoveredBy);

protected:
   static bool coveredBy(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;

};

class Touches : public SpatialRelationOperation
{
public:
    Touches();

    Touches(quint64 metaid, const Ilwis::OperationExpression &expr);

    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

   NEW_OPERATION(Touches);

protected:
   static bool touches(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;

};

class Intersects : public SpatialRelationOperation
{
public:
    Intersects();

    Intersects(quint64 metaid, const Ilwis::OperationExpression &expr);

    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

   NEW_OPERATION(Intersects);

protected:
   static bool intersects(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;

};

class Disjoint : public SpatialRelationOperation
{
public:
    Disjoint();

    Disjoint(quint64 metaid, const Ilwis::OperationExpression &expr);

    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

   NEW_OPERATION(Disjoint);

protected:
   static bool disjoint(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;

};

class Within : public SpatialRelationOperation
{
public:
    Within();

    Within(quint64 metaid, const Ilwis::OperationExpression &expr);

    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

   NEW_OPERATION(Within);

protected:
   static bool within(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;

};

class Equals : public SpatialRelationOperation
{
public:
    Equals();

    Equals(quint64 metaid, const Ilwis::OperationExpression &expr);

    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

   NEW_OPERATION(Equals);

protected:
   static bool equals(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;

};

class Crosses : public SpatialRelationOperation
{
public:
    Crosses();

    Crosses(quint64 metaid, const Ilwis::OperationExpression &expr);

    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

   NEW_OPERATION(Crosses);

protected:
   static bool crosses(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;

};

class Overlaps : public SpatialRelationOperation
{
public:
    Overlaps();

    Overlaps(quint64 metaid, const Ilwis::OperationExpression &expr);

    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

   NEW_OPERATION(Overlaps);

protected:
   static bool overlaps(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;

};
}
}

#endif // SPATIALRELATION_H
