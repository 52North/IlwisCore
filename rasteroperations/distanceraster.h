#ifndef DISTANCERASTER_H
#define DISTANCERASTER_H


namespace Ilwis {
namespace RasterOperations {

class DistanceRaster : public OperationImplementation
{
public:
    DistanceRaster();
    DistanceRaster(quint64 metaid, const Ilwis::OperationExpression &expr);

    ~DistanceRaster();

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);
    static quint64 createMetadata();

private:
    bool isValueMap();
    bool isItemMap();

    IRasterCoverage _inputRaster;
    IRasterCoverage _inputWeightRaster; // optional
    IRasterCoverage _inputThiessenRaster; // optional
    IRasterCoverage _outputRaster;

    bool            _hasWeightRaster;

    void ThiessenMapCalculation();
    void distanceCalculation();

    NEW_OPERATION(DistanceRaster);
    bool setDistanceValue(PixelIterator iter, PixelIterator neighbour, Size<> sz, double weight);
};

}
}


#endif // DISTANCERASTER_H
