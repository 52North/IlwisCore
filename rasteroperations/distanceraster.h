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

    enum CalcDirection{cdForward, cdBackward};

private:
    bool isValueMap();
    bool isItemMap();

    IRasterCoverage _inputRaster;
    IRasterCoverage _inputOptWeightRaster;
    IRasterCoverage _inputThiessenRaster; // optional
    IRasterCoverage _outputRaster;

    bool            _hasWeightRaster;

    void ThiessenMapCalculation();
    void distanceCalculation();

    NEW_OPERATION(DistanceRaster);
    bool setDistanceValue(PixelIterator & iter, PixelIterator & neighbour, Size<> sz, double weight, CalcDirection cd);
    double Min(double val1, double val2);
};

}
}


#endif // DISTANCERASTER_H
