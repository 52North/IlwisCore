#ifndef OPERATIONHELPERFEATURES_H
#define OPERATIONHELPERFEATURES_H

namespace Ilwis {

typedef  std::function<bool(const std::vector<quint32>&)> SubSetAsyncFunc;

class KERNELSHARED_EXPORT OperationHelperFeatures
{
public:
    OperationHelperFeatures();
    static IIlwisObject initialize(const IIlwisObject &inputObject, IlwisTypes tp, quint64 what);
    static int subdivideTasks(ExecutionContext *ctx, const IFeatureCoverage& gcov, std::vector<std::vector<quint32> > &subsets);
    template<typename T> static bool execute(ExecutionContext* ctx, T func, IFeatureCoverage& inputFC){
        std::vector<std::vector<quint32>> subsets;

        int cores = OperationHelperFeatures::subdivideTasks(ctx,inputFC, subsets);
        if ( cores == iUNDEF)
            return false;

        std::vector<std::future<bool>> futures(cores);
        bool res = true;

        for(int i =0; i < cores; ++i) {
            futures[i] = std::async(std::launch::async, func, subsets[i]);
        }

        for(int i =0; i < cores; ++i) {
            res &= futures[i].get();
        }
        return true;
    }
};
}

#endif // OPERATIONHELPERFEATURES_H
