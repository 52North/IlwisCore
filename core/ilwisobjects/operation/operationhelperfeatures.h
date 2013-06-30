#ifndef OPERATIONHELPERFEATURES_H
#define OPERATIONHELPERFEATURES_H

namespace Ilwis {

typedef  std::function<bool(const std::vector<quint32>&)> SubSetAsyncFunc;

class OperationHelperFeatures
{
public:
    OperationHelperFeatures();
    IIlwisObject initialize(const IIlwisObject &inputObject, IlwisTypes tp, quint64 what);
};
}

#endif // OPERATIONHELPERFEATURES_H
