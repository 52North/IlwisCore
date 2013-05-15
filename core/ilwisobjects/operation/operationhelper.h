#ifndef OPERATIONHELPER_H
#define OPERATIONHELPER_H

namespace Ilwis {
class KERNELSHARED_EXPORT OperationHelper
{
public:
    OperationHelper();
    static Box3D<qint32> initialize(const IGridCoverage &inputGC, IGridCoverage &outputGC,  const Parameter &parm, quint64 what);
    static bool splitBoxY(int number, const Box3D<qint32> startBox, std::vector<Box3D<qint32> > &boxes);
};
}

#endif // OPERATIONHELPER_H
