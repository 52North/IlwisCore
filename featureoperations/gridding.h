#ifndef GRIDDING_H
#define GRIDDING_H

namespace Ilwis {
namespace FeatureOperations {
class Gridding : public OperationImplementation
{
public:
    Gridding();
    Gridding(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    State prepare(ExecutionContext *ctx, const SymbolTable& sym);

private:
    ICoordinateSystem _csy;
    IFeatureCoverage _outfeatures;
    ITable _attTable;
    Coordinate _top;
    double _cellXSize;
    double _cellYSize;
    quint32 _xsize;
    quint32 _ysize;
};
}
}

#endif // GRIDDING_H
