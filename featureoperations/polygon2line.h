#ifndef POLYGON2LINE_H
#define POLYGON2LINE_H


namespace Ilwis {
namespace FeatureOperations {

class Polygon2Line : public OperationImplementation
{
public:
    Polygon2Line();
    ~Polygon2Line();
    Polygon2Line(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
    IFeatureCoverage _outputfeatures;
    IFeatureCoverage _inputfeatures;
    bool _singleId;
    NEW_OPERATION(Polygon2Line);
    void extractBoundary(const Ilwis::UPGeometry &g, quint32 &record);
};
}
}

#endif // POLYGON2LINE_H
