#include "kernel.h"
#include "ilwisdata.h"
#include "ilwisobject.h"
#include "operationmetadata.h"
#include "operationExpression.h"
#include "connectorinterface.h"
#include "commandhandler.h"
#include "operation.h"

using namespace Ilwis;



OperationImplementation::OperationImplementation(quint64 metaid, const Ilwis::OperationExpression &e) : _prepState(sNOTPREPARED)
{
   _expression = e;
   if(!_metadata.prepare(metaid)) {
      ERROR2(ERR_NO_INITIALIZED_2,"metadata",e.name());
   }
}


const IOperationMetaData &OperationImplementation::metadata() const
{
    return _metadata;
}

//-----------------------------------------------------------------------------
Operation::Operation(const Ilwis::OperationExpression &e)
{
    _operation.reset(commandhandler()->create(e));

}

UPOperationImplementation& Operation::operator->(){
    if ( _operation)
        return _operation;
    throw ErrorObject(TR("Using uninitialized operation"));
}

const UPOperationImplementation& Operation::operator->() const{
    if ( _operation)
        return _operation;
    throw ErrorObject(TR("Using uninitialized operation"));
}

bool Operation::isValid() const
{
    if(!_operation)
        return false;
    return _operation->isValid();
}


bool OperationImplementation::isValid() const
{
    return _expression.isValid() && _metadata.isValid();
}




