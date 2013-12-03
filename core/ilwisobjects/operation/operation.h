#ifndef OPERATION_H
#define OPERATION_H

namespace Ilwis {

class OperationExpression;
class SymbolTable;

class KERNELSHARED_EXPORT OperationImplementation : public Identity
{
public:
    enum State{sNOTPREPARED,sPREPARED, sPREPAREFAILED};
    OperationImplementation() : _prepState(sNOTPREPARED) {}
    OperationImplementation(quint64 metaid, const Ilwis::OperationExpression &e);
    virtual ~OperationImplementation() {}
    const IOperationMetaData& metadata() const;
    virtual bool execute(ExecutionContext *ctx, SymbolTable& symTable)=0;
    virtual bool isValid() const;
    OperationExpression expression() const;

protected:
    IOperationMetaData _metadata;
    OperationExpression _expression;
    State _prepState;

    virtual State prepare(ExecutionContext *ctx, const SymbolTable& symTable) =0;
};

typedef QScopedPointer<OperationImplementation> SPOperationImplementation;

class KERNELSHARED_EXPORT Operation
{
public:
    Operation() {}
    virtual ~Operation();

    Operation(const Ilwis::OperationExpression &e);

    Operation& operator=(const Operation& op);
    SPOperationImplementation& operator->();
    const SPOperationImplementation& operator->() const;
    bool isValid() const;
    template<typename T> static T calculate(const QString& name, const QString& txt){
        SymbolTable symtbl;
        bool ok = false;
        ExecutionContext ctx;
        OperationExpression o("script " + txt);
        Operation op(o);
        try{
            ok =  op->execute(&ctx,symtbl);
            if(!ok)
                return T();
            QVariant var = symtbl.getValue(name);
            if ( var.isValid()) {
                return var.value<T>();
            }
        } catch (const ErrorObject& err) {
            kernel()->issues()->log(err.message());
            return T();
        }
        return T();
    }

private:
    SPOperationImplementation _operation;


};
typedef std::function<Ilwis::OperationImplementation *(quint64 metaid, const OperationExpression&)> CreateOperation;
}



#endif // OPERATION_H
