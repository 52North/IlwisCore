#ifndef OPERATIONHELPERFEATURES_H
#define OPERATIONHELPERFEATURES_H

namespace Ilwis {

typedef  std::function<bool(const std::vector<quint32>&)> SubSetAsyncFunc;

class KERNELSHARED_EXPORT OperationHelperFeatures
{
public:
    OperationHelperFeatures();
    static IIlwisObject initialize(const IIlwisObject &inputObject, IlwisTypes tp, quint64 what);
    template<typename T> static bool execute(ExecutionContext* ctx, T func, IFeatureCoverage& outputGC, std::vector<quint32> subset=std::vector<quint32>()){
        return false;
    }
};
}

#endif // OPERATIONHELPERFEATURES_H
