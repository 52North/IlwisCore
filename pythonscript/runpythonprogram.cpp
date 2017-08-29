#include <cmath>
#include "Python.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "catalog.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "ilwiscontext.h"
#include "runpython.h"
#include "runpythonprogram.h"

using namespace Ilwis;
using namespace PythonScript;

RunPythonProgram::RunPythonProgram()
{

}

RunPythonProgram::RunPythonProgram(quint64 metaid, const Ilwis::OperationExpression &expr) : RunPython(metaid,expr)
{

}

OperationImplementation *RunPythonProgram::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RunPythonProgram(metaid, expr);
}

quint64 RunPythonProgram::createMetadata()
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

OperationImplementation::State RunPythonProgram::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString pythonfile(_expression.input<QString>(0));
    if ( pythonfile.indexOf("file://") == -1){
        QString path = context()->workingCatalog()->resource().url().toString();
        pythonfile += path + "/" + pythonfile;
    }

    QFile file(pythonfile);
    if ( !file.exists()){
        kernel()->issues()->log(TR("Python file doesnt exist:") + _expression.input<QString>(0));
        return sPREPAREFAILED;
    }
    if ( !file.open(QIODevice::ReadOnly|QIODevice::Text)){
        kernel()->issues()->log(TR("Python file can not be opened:") + _expression.input<QString>(0));
        return sPREPAREFAILED;
    }

    QString filecontent = file.readAll();

    _statements = filecontent;

    return sPREPARED;
}


