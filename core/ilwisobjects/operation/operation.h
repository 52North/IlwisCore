#ifndef OPERATION_H
#define OPERATION_H

#include "tranquilizer.h"

namespace Ilwis {

class OperationExpression;
class SymbolTable;

class KERNELSHARED_EXPORT OperationImplementation : public Identity
{
public:
    enum State{sNOTPREPARED,sPREPARED, sPREPAREFAILED};
    OperationImplementation() : _prepState(sNOTPREPARED) {}
    OperationImplementation(quint64 metaid, const Ilwis::OperationExpression &e);
    virtual ~OperationImplementation() ;
    const IOperationMetaData& metadata() const;
    UPTranquilizer& trq();
    virtual bool execute(ExecutionContext *ctx, SymbolTable& symTable)=0;
    virtual bool isValid() const;
    OperationExpression expression() const;
    void updateTranquilizer(quint64 currentCount, quint32 step){
        if ( (currentCount % step) == 0){
            trq()->update(step);
        }
    }
    void logOperation(const IIlwisObject& obj, const OperationExpression& expr);
    void logOperation(const OperationExpression &expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

protected:
    void initialize(quint64 totalCoun);
    IOperationMetaData _metadata;
    OperationExpression _expression;
    State _prepState;
    std::unique_ptr<Tranquilizer> _tranquilizer;
    template<class T> void setOutput(const T& obj, ExecutionContext *ctx, SymbolTable& symTable){
        QVariant v;
        v.setValue(obj);
        ctx->setOutput(symTable, v, obj->name(), obj->ilwisType(), obj->resource());
    }

    template<typename T, typename S> bool compare1(LogicalOperator oper, const T& v1, const S& v2){
        switch(oper) {
        case loEQ:
            return v1 == v2;
        case loLESS:
            return v1 < v2;
        case loLESSEQ:
            return v1 <= v2;
        case loNEQ:
            return v1 != v2;
        case loGREATER:
            return v1 > v2;
        case loGREATEREQ:
            return v1 >= v2;
        default:
            return false;
        }
    }

    double compare2(LogicalOperator oper, const double& v1, const double& v2, bool isNumeric){
        switch(oper) {
        case loAND:
            if (!(bool)v1 || !(bool)v2) // FALSE AND UNDEF ==> FALSE
                return false;
            else if (v1 == rUNDEF || v2 == rUNDEF) // TRUE AND UNDEF ==> UNDEF
                return rUNDEF;
            else
                return true; // Remaining case: TRUE AND TRUE
        case loOR:
            if (v1 == rUNDEF) {
                if (v2 == rUNDEF) // UNDEF OR UNDEF ==> UNDEF
                    return rUNDEF;
                else // UNDEF OR TRUE ==> TRUE
                    return (bool)v2 ? true : rUNDEF;
            } else if (v2 == rUNDEF) // TRUE OR UNDEF ==> TRUE
                return (bool)v1 ? true : rUNDEF;
            else // Normal case: OR
                return (bool)v1 || (bool)v2;
        case loXOR:
            if (v1 == rUNDEF || v2 == rUNDEF) // A XOR UNDEF ==> UNDEF
                return rUNDEF;
            else
                return ((bool)v1) ^ ((bool)v2); // Normal case: XOR
        case loEQ:
            return (isNumeric || (v1 != rUNDEF && v2 != rUNDEF)) ? (v1 == v2) : rUNDEF; // Allow comparison of (raster,value) with rUNDEF, for ifundef
        case loNEQ:
            return (isNumeric || (v1 != rUNDEF && v2 != rUNDEF)) ? (v1 != v2) : rUNDEF; // Allow comparison of (raster,value) with rUNDEF, for ifnotundef
        default:
            if (v1 == rUNDEF || v2 == rUNDEF)
                return rUNDEF;
            else
                return compare1(oper, v1, v2);
        }
    }
};

typedef QScopedPointer<OperationImplementation> SPOperationImplementation;
typedef std::function<Ilwis::OperationImplementation *(quint64 metaid, const OperationExpression&)> CreateOperation;

class KERNELSHARED_EXPORT Operation
{
public:
    Operation() {}
    virtual ~Operation();

    Operation(const Ilwis::OperationExpression &e);
    template<typename ...Parms> Operation(const std::vector<QString>& outputnames, const QString& operation, Parms ...parms){
        QString expression;
        for(auto name : outputnames){
            if ( expression != "")
                expression += ",";
            expression += name;
        }
        if ( expression != "")
            expression += "=";
        expression += operation;
        expression += "(" + splitInParameters(parms...) + ")";
        OperationExpression operationexpr(expression);
        _operation.reset(commandhandler()->create(operationexpr));

    }

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




    // helper function
    static std::nullptr_t registerOperation(quint64 id, Ilwis::CreateOperation op);

private:
    template<typename T, typename ...Parms> QString splitInParameters(const T& parameter, Parms ...rest){
        QString s = stringRepresentation(parameter);
        QString nextParameters = splitInParameters(rest...);
        if ( nextParameters != "")
            s += "," + nextParameters;

        return s;
    }

    QString splitInParameters(){
        return "";
    }

    template<typename T> QString stringRepresentation(const T& var){
        return "";
    }

    template<typename T> QString stringRepresentation(const T* var){
        return var; // assuming char *; else the compiler will anyway generate an error and a specialization is needed
    }

    SPOperationImplementation _operation;


};



}

#define NEW_OPERATION(name) \
    private: \
static name *dummy_operation;

#define REGISTER_OPERATION(name) \
    name *name::dummy_operation = Operation::registerOperation(name::createMetadata(),name::create);



#endif // OPERATION_H
