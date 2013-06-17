#ifndef OPERATION_H
#define OPERATION_H

namespace Ilwis {

class KERNELSHARED_EXPORT OperationImplementation : public Identity
{
public:
    enum State{sNOTPREPARED,sPREPARED, sPREPAREFAILED};
    OperationImplementation() : _prepState(sNOTPREPARED) {}
    OperationImplementation(quint64 metaid, const Ilwis::OperationExpression &e);
    virtual ~OperationImplementation() {}
    const IOperationMetaData& metadata() const;
    virtual bool execute(ExecutionContext *ctx)=0;
    virtual bool isValid() const;

protected:
    IOperationMetaData _metadata;
    OperationExpression _expression;
    State _prepState;

    virtual State prepare() =0;
};

typedef QScopedPointer<OperationImplementation> SPOperationImplementation;

class KERNELSHARED_EXPORT Operation
{
public:
    Operation() {}
    virtual ~Operation();

    Operation(const Ilwis::OperationExpression &e);
    SPOperationImplementation& operator->();
    const SPOperationImplementation& operator->() const;
    bool isValid() const;

private:
    SPOperationImplementation _operation;


};
typedef std::function<Ilwis::OperationImplementation *(quint64 metaid, const OperationExpression&)> CreateOperation;
}



#endif // OPERATION_H
