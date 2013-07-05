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
    template<typename T> static bool execute(ExecutionContext* ctx, T func, IFeatureCoverage& inputFC, IFeatureCoverage& outputFC){
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

    template<typename T> static bool execute(ExecutionContext* ctx, T func, IFeatureCoverage& inputFC, IFeatureCoverage& outputFC, ITable& tbl){
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

        if ( res && outputFC.isValid()) {
            //TODO better handling for multiple feature types
            if ( !tbl.isValid())
                return false;
            for(int i=0; i < tbl->columns(); ++i ){
                ColumnDefinition& def = tbl->columndefinition(i);
                if ( def.datadef().domain()->valueType() & itNUMERIC) {
                    ContainerStatistics<double> stats;
                    std::vector<QVariant> values = tbl->column(i);
                    std::vector<double> vec(values.size());
                    for(int i=0; i < vec.size(); ++i) {
                        vec[i] = values[i].toDouble();
                    }
                    stats.calculate(vec.begin(), vec.end());
                    NumericRange *rng = new NumericRange(stats.min(), stats.max(), std::pow(10,-stats.significantDigits()));
                    def.datadef().range(rng,DataDefinition::daCOVERAGE);
                }
            }
        }
        return true;
    }
};
}

#endif // OPERATIONHELPERFEATURES_H
