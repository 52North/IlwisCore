#include <cmath>
#include "Python.h"
#include <map>
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "tranquilizer.h"
#include "callpythonoperation.h"

using namespace Ilwis;
using namespace PythonScript;

std::map<QString, QString> CallPythonOperation::_pythonoperations;

REGISTER_OPERATION(CallPythonOperation)

CallPythonOperation::CallPythonOperation()
{

}

CallPythonOperation::CallPythonOperation(quint64 metaid, const Ilwis::OperationExpression &expr): OperationImplementation(metaid,expr)
{

}

bool CallPythonOperation::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED){
            return false;
        }
      PyObject *pGlobal = PyDict_New();

      //Create a new module object
      PyObject *pNewMod = PyModule_New("pythonscriptmodule");

      Py_Initialize();
      PyModule_AddStringConstant(pNewMod, "__file__", "");

      //Get the dictionary object from my module so I can pass this to PyRun_String
      PyObject *pLocal = PyModule_GetDict(pNewMod);

      //Define my function in the newly created module
      PyObject *pValue = PyRun_String(_pythonText.toLatin1(), Py_file_input, pGlobal, pLocal);
      Py_DECREF(pValue);

      //Get a pointer to the function I just defined
      PyObject *pFunc = PyObject_GetAttrString(pNewMod, _expression.name().toLatin1());

      //Build a tuple to hold my arguments (just the number 4 in this case)
      std::vector<PyObject *> values;
      PyObject *pArgs = PyTuple_New(_expression.parameterCount());
      for(int i=1; i<= _expression.parameterCount(); ++i){
        PyObject *pValue = PyLong_FromLong(4);
        PyTuple_SetItem(pArgs, i, pValue);
        values.push_back(pValue);
      }

      //Call my function, passing it the number four
      pValue = PyObject_CallObject(pFunc, pArgs);
      Py_DECREF(pArgs);

      //printf("Returned val: %ld\n", PyLong_AsLong(pValue));
      for(auto *p : values)
        Py_DECREF(p);

      Py_XDECREF(pFunc);
      Py_DECREF(pNewMod);
      Py_Finalize();

      return 0;

    return true;
}

Ilwis::OperationImplementation *CallPythonOperation::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
   return new CallPythonOperation(metaid,expr);
}

Ilwis::OperationImplementation::State CallPythonOperation::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    QString operName =_expression.name(true);
    auto iter = _pythonoperations.find(operName);
    if ( iter == _pythonoperations.end()){
        kernel()->issues()->log(TR("Couldn't find operation ") + operName);
        return sPREPAREFAILED;
    }
    QFileInfo fileinf = (*iter).second;
    if (!fileinf.exists()){
        kernel()->issues()->log(TR("Couldn't find python file ") + fileinf.absoluteFilePath());
        return sPREPAREFAILED;
    }

    QFile file(fileinf.absoluteFilePath());
    if (!file.open(QFile::ReadOnly)){
        kernel()->issues()->log(TR("Couldn't open python file ") + fileinf.absoluteFilePath());
        return sPREPAREFAILED;
    }

    _pythonText = file.readAll();

    return sPREPARED;
}

quint64 CallPythonOperation::createMetadata()
{
    OperationResource operation({"ilwis://operations/callpython"});
    operation.setLongName("Call Python");
    operation.setSyntax("callpython()");
    operation.setKeywords("internal,python");
    operation.setDescription(TR("Wrapper for calls to python"));
    operation.setInParameterCount({0});
    operation.setOutParameterCount({0});
    mastercatalog()->addItems({operation});
    return operation.id();
}

void CallPythonOperation::addOperation(const QString &name, const QString &filename)
{
    _pythonoperations[name] = filename;
}

