#include "kernel.h"
#include "ilwisdata.h"
#include "ilwisobject.h"
#include "operationmetadata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "connectorinterface.h"
#include "commandhandler.h"
#include "ilwiscontext.h"
#include "operation.h"

using namespace Ilwis;

OperationImplementation::   OperationImplementation(quint64 metaid, const Ilwis::OperationExpression &e) : _prepState(sNOTPREPARED)
{
   _expression = e;
   if(!_metadata.prepare(metaid)) {
      ERROR2(ERR_NO_INITIALIZED_2,"metadata",e.name());
   }
}

OperationImplementation::~OperationImplementation()
{
    if ( _tranquilizer){
        _tranquilizer->stop();
    }
}


const IOperationMetaData &OperationImplementation::metadata() const
{
    return _metadata; //nop
}

UPTranquilizer &OperationImplementation::trq()
{
    if ( _tranquilizer)
        return _tranquilizer;
    throw ErrorObject(TR("Tranquilizer not properly initialized"));
}

OperationExpression OperationImplementation::expression() const
{
    return _expression;
}

void OperationImplementation::logOperation(const IIlwisObject &obj, const OperationExpression &expr)
{
    if (obj.isValid()){
        obj->addDescription(expr.toString());
    }
}

void OperationImplementation::logOperation(const OperationExpression &expr)
{
    QString message = expr.toPythonExpression();
    kernel()->issues()->log(message, IssueObject::itExpression);
}

OperationImplementation::State OperationImplementation::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    logOperation(_expression);
    return sPREPARED;
}

void OperationImplementation::initialize(quint64 totalCount)
{
    if ( totalCount != i64UNDEF){
        if (!_tranquilizer){
            _tranquilizer.reset(Tranquilizer::create(context()->runMode()));
        }
    }
    _tranquilizer->prepare(_metadata->name(), _metadata->description(), totalCount);

}

bool OperationImplementation::isValid() const
{
    return _expression.isValid() && _metadata.isValid();
}



//-----------------------------------------------------------------------------
Operation::Operation(const Ilwis::OperationExpression &e)
{
    _operation.reset(commandhandler()->create(e));

}

Operation& Operation::operator=(const Operation& op){
    if (op.isValid())
        _operation.reset(commandhandler()->create(op._operation->expression()));
    return *this;
}

Operation::~Operation() {
}

SPOperationImplementation& Operation::operator->(){
    if (! _operation.isNull())
        return _operation;
    throw ErrorObject(TR("Using uninitialized operation"));
}

const SPOperationImplementation& Operation::operator->() const{
    if ( !_operation.isNull())
        return _operation;
    throw ErrorObject(TR("Using uninitialized operation"));
}

bool Operation::isValid() const
{
    if(!_operation)
        return false;
    return _operation->isValid();
}

std::nullptr_t Operation::registerOperation(quint64 id, CreateOperation op)
{
    Ilwis::commandhandler()->addOperation(id, op);
    return nullptr;
}








