#include <cmath>
#include "Python.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "ilwiscontext.h"
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

    QString tempPath = context()->cacheLocation().toLocalFile();
    tempPath += "/python.stdout";
    QString cmd = QString("f = open(\"%1\", \"w\", encoding=\"utf-8\")").arg(tempPath);
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import io");
    PyRun_SimpleString("stdout = sys.stdout");
    PyRun_SimpleString("stderr = sys.stderr");
    PyRun_SimpleString(cmd.toStdString().c_str());
    PyRun_SimpleString("sys.stdout = f");
    PyRun_SimpleString(_statements.toStdString().c_str());
    PyRun_SimpleString("\n");
    PyRun_SimpleString("sys.stdout.flush()");
    PyRun_SimpleString("sys.stdout = stdout");
    PyRun_SimpleString("sys.stderr = stderr");
    PyRun_SimpleString("f.close()");



    QFile inputFile(tempPath);
    if ( inputFile.exists()){
        if ( inputFile.open(QIODevice::ReadOnly)){
            QString result(inputFile.readAll());
            result.remove("\r\n");
            ctx->setOutput(symTable,result,"python_output", itSTRING, Resource());
            inputFile.close();
        }
    }
    inputFile.remove();


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
    QString exp(_expression.input<QString>(0));
    if ( exp[0] == '\"' && exp[exp.size() - 1] == '\"'){
        exp = exp.mid(1,exp.size() - 2);
    }
    _statements = exp;

    return sPREPARED;
}

