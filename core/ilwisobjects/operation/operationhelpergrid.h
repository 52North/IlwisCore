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

        double minv=1e307,maxv = -1e307;
        double minBand = minv, maxBand = maxv;
        if ( res && outputRaster.isValid()) {
            if ( outputRaster->datadef().domain<>()->valueType() & itNUMBER) {
                PixelIterator iter(outputRaster);
                auto end = iter.end();
                while(iter != end){
                    double v = *iter;
                    minv = Ilwis::min(v,minv);
                    maxv = Ilwis::max(v,maxv);
                    minBand = Ilwis::min(v,minv);
                    maxBand = Ilwis::max(v,maxv);
                    ++iter;
                    if ( iter.zchanged()){
                        outputRaster->datadefRef(iter.z()).range<NumericRange>()->max(maxBand);
                        outputRaster->datadefRef(iter.z()).range<NumericRange>()->min(minBand);
                        minBand=1e307;
                        maxBand = -1e307;
                    }
                }
                double resolution = outputRaster->datadefRef().range<NumericRange>()->resolution();
                NumericRange *rng = new NumericRange(minv, maxv, resolution);
                for(int i=0; i < outputRaster->size().zsize(); ++i){
                    outputRaster->datadefRef(i).range<NumericRange>()->resolution(resolution)    ;
                }
                outputRaster->datadefRef().range(rng);
            }
        }
        return res;
    }
    static IIlwisObject initialize(const IIlwisObject &inputObject, IlwisTypes tp, quint64 what);
};
}

#endif // OPERATIONHELPER_H
