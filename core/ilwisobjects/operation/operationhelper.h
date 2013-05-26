#ifndef OPERATIONHELPER_H
#define OPERATIONHELPER_H



namespace Ilwis {

typedef  std::function<bool(const Box3D<qint32>&)> BoxedAsync;

class KERNELSHARED_EXPORT OperationHelper
{
public:
    OperationHelper();
    static Box3D<qint32> initialize(const IGridCoverage &inputGC, IGridCoverage &outputGC,  const Parameter &parm, quint64 what);
    static int subdivideTasks(const IGridCoverage& gcov, std::vector<Box3D<qint32> > &boxes);

    template<typename T> static bool execute(T func, IGridCoverage& outputGC) {
        std::vector<Box3D<qint32>> boxes;

        int cores = OperationHelper::subdivideTasks(outputGC,boxes);

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
        return res;
    }
};
}

#endif // OPERATIONHELPER_H
