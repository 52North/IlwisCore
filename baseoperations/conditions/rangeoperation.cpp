#include "kernel.h"
#include "ilwisdata.h"
#include "kernel.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "rangeoperation.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(RangeOperation)


Ilwis::OperationImplementation *RangeOperation::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RangeOperation(metaid, expr);
}

RangeOperation::RangeOperation()
{
}

RangeOperation::RangeOperation(quint64 metaid,const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool RangeOperation::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    return true;
}

OperationImplementation::State RangeOperation::prepare(ExecutionContext *ctx,const SymbolTable&)
{
   return sPREPARED;
}


quint64 RangeOperation::createMetadata()
{
    OperationResource operation({"ilwis://operations/range"});
    operation.setLongName("Range");
    operation.setSyntax("range(number|string|band|polygon|line|point|feature|record,container)");
    operation.setDescription(TR("Placeholder operation for use in workflows. It enables the range interface in loops"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itSTRING, TR("RangeOperation variable"),TR("Variable name that indicates the type of value that is used in the iteration"));
    operation.addInParameter(1,itSTRING|itCOVERAGE|itTABLE, TR("Data container"),TR("Any object that describes a set of objects"));
    operation.setOutParameterCount({0});
    operation.setKeywords("tests, workflow");

    mastercatalog()->addItems({operation});
    return operation.id();
}


