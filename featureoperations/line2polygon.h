#ifndef LINE2POLYGON_H
#define LINE2POLYGON_H


namespace Ilwis {
namespace FeatureOperations {

class Line2Polygon : public OperationImplementation
{
public:
    Line2Polygon();
    Line2Polygon(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
    IFeatureCoverage _outputfeatures;
    IFeatureCoverage _inputfeatures;
    NEW_OPERATION(Line2Polygon);
};
}
}


#endif // LINE2POLYGON_H
