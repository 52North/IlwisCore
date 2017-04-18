#ifndef APPLYCOMBINATIONMATRIX_H
#define APPLYCOMBINATIONMATRIX_H


namespace Ilwis {
namespace RasterOperations {


class ComboClassification : public OperationImplementation
{
public:
    ComboClassification();

    ComboClassification(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
    IRasterCoverage _inputRasterX;
    IRasterCoverage _inputRasterY;
    IRasterCoverage _outputRaster;
    ICombinationMatrix _comboMatrix;
    BoundingBox _box;

    NEW_OPERATION(ComboClassification);
    std::vector<int> raw2index(CombinationMatrix::Axis axis);
};
}
}

#endif // APPLYCOMBINATIONMATRIX_H
