#ifndef OPERATIONHELPERRASTER_H
#define OPERATIONHELPERRASTER_H

namespace Ilwis {

typedef  std::function<bool(const Box3D<qint32>&)> BoxedAsyncFunc;

class KERNELSHARED_EXPORT OperationHelperRaster
{
public:
    OperationHelperRaster();
    static Box3D<qint32> initialize(const IGridCoverage &inputGC, IGridCoverage &outputGC, const Ilwis::Parameter &parm, quint64 what);
    static int subdivideTasks(ExecutionContext *ctx,const IGridCoverage& gcov, const Box3D<qint32>& bounds, std::vector<Box3D<qint32> > &boxes);

    template<typename T> static bool execute(ExecutionContext* ctx, T func, IGridCoverage& outputGC, const Box3D<qint32>& bounds=Box3D<qint32>()) {
        std::vector<Box3D<qint32>> boxes;

        int cores = OperationHelperRaster::subdivideTasks(ctx,outputGC,bounds, boxes);

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

        if ( res && outputGC.isValid()) {
            if ( outputGC->datadef().domain()->valueType() & itNUMERIC) {
                CoverageStatistics stats;
                PixelIterator iter(outputGC);
                stats.calculate(iter, iter.end());
                NumericRange *rng = new NumericRange(stats.min(), stats.max(), std::pow(10,-stats.significantDigits()));
                outputGC->datadef().range(rng,DataDefinition::daCOVERAGE);
            }
        }
        return res;
    }
    static IIlwisObject initialize(const IIlwisObject &inputObject, IlwisTypes tp, quint64 what);
};
}

#endif // OPERATIONHELPER_H
