#ifndef OPERATIONHELPERRASTER_H
#define OPERATIONHELPERRASTER_H

namespace Ilwis {

typedef  std::function<bool(const BoundingBox&)> BoxedAsyncFunc;
class Parameter;

class KERNELSHARED_EXPORT OperationHelperRaster
{
public:
    OperationHelperRaster();
    static BoundingBox initialize(const IRasterCoverage &inputRaster, IRasterCoverage &outputRaster, quint64 what);
    static int subdivideTasks(ExecutionContext *ctx,const IRasterCoverage& raster, const BoundingBox& bounds, std::vector<BoundingBox > &boxes);
    static bool resample(IRasterCoverage& input1, IRasterCoverage& input2, ExecutionContext *ctx);

    template<typename T> static bool execute(ExecutionContext* ctx, T func, IRasterCoverage& outputRaster, const BoundingBox& bounds=BoundingBox()) {
        std::vector<BoundingBox> boxes;

        int cores = OperationHelperRaster::subdivideTasks(ctx,outputRaster,bounds, boxes);

        if ( cores == iUNDEF)
            return false;

        std::vector<std::future<bool>> futures(cores);
        bool res = true;

        for(int i =0; i < cores; ++i) {
            futures[i] = std::async(std::launch::async, func, boxes[i]);
        }

        for(int i =0; i < cores; ++i) {
            res &= futures[i].get();
        }

        if ( res && outputRaster.isValid()) {
            if ( outputRaster->datadef().domain<>()->valueType() & itNUMBER) {
                NumericStatistics stats;
                PixelIterator iter(outputRaster);
                stats.calculate(iter, iter.end());
                NumericRange *rng = new NumericRange(stats[NumericStatistics::pMIN], stats[NumericStatistics::pMAX], std::pow(10,-stats.significantDigits()));
                outputRaster->datadefRef().range(rng);
            }
        }
        return res;
    }
    static IIlwisObject initialize(const IIlwisObject &inputObject, IlwisTypes tp, quint64 what);
};
}

#endif // OPERATIONHELPER_H
