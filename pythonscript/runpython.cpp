#include <cmath>
#include "Python.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "runpython.h"

using namespace Ilwis;
using namespace PythonScript;

RunPython::RunPython()
{

}

RunPython::RunPython(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid,expr)
{

}

RunPython::~RunPython()
{

}

bool RunPython::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    PyRun_SimpleString(_statements.toStdString().c_str());
    return true;
}

OperationImplementation *RunPython::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RunPython(metaid, expr);
}

quint64 RunPython::createMetadata()
{
    OperationResource operation({"ilwis://operations/runpython"});
    operation.setSyntax("runpython(expression");
    operation.setLongName("Run a python expression");
    operation.setDescription(TR("runs one or more python statements and returns a string referencing their output"));
    operation.setInParameterCount({1});
    operation.addInParameter(0,itSTRING, TR("python statements"),TR("one or more lines of python code"));

    operation.setOutParameterCount({0,1});
    operation.addOutParameter(0,itSTRING, TR("output"),TR("optional output of the executed python lines") );
    operation.setKeywords("python, script");

    mastercatalog()->addItems({operation});
    return operation.id();
}

OperationImplementation::State RunPython::prepare(ExecutionContext *, const SymbolTable &)
{
    _statements = _expression.input<QString>(0)    ;

    return sPREPARED;
}

