#ifndef OPERATIONCONNECTOR_H
#define OPERATIONCONNECTOR_H

namespace Ilwis {
class KERNELSHARED_EXPORT OperationConnector : public IlwisObjectConnector
{
public:
    OperationConnector();

    virtual bool canUse(const OperationExpression&) const = 0;

};
}

#endif // OPERATIONCONNECTOR_H
