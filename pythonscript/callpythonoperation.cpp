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
bool CallPythonOperation::_pathsAdded = false;

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

  //  PyRun_SimpleString("import sys\n");
  //  QString path = "sys.path.append(\"" + _pythonFile.absolutePath() + "\")";
    //PyRun_SimpleString(path.toLatin1());//the folder where the pythonTest.py is located

    QString basname = _pythonFile.baseName();
    PyObject  *pName = PyUnicode_FromString(basname.toLatin1());//creates new reference so you have to DECREF if

    PyObject *pModule = PyImport_Import(pName);//import module pythonTest.py. New reference

    PyObject *pDict = PyModule_GetDict(pModule);//borowed reference so no DECREF

      ///function call and return test
      QString operName =_expression.name(true);
      PyObject *pFunc = PyDict_GetItemString(pDict, operName.toLatin1());//borowed reference. choose file is the name of the funcion

      //Build a tuple to hold my arguments
      std::vector<PyObject *> values;
      PyObject *pArgs = PyTuple_New(_expression.parameterCount());
      for(int i=0; i< _expression.parameterCount(); ++i){
        Parameter parm = _expression.parm(i);
        PyObject *pValue;
        if ( hasType(parm.valuetype(), itINTEGER)){
            pValue = PyLong_FromLong(_expression.parm(i).value().toLong());
        }else if ( hasType(parm.valuetype(),itFLOAT | itDOUBLE|itDATETIME)){
            pValue = PyFloat_FromDouble(_expression.input<double>(i));
        } else{
           QByteArray byteArray = parm.value().toUtf8();
           const char *cString = byteArray.constData();
           pValue = PyUnicode_FromString(cString);
        }

        PyTuple_SetItem(pArgs, i, pValue);
        values.push_back(pValue);
      }



      //Call my function, passing it the number four
      PyObject *pValue = PyObject_CallObject(pFunc, pArgs);

      if (PyLong_Check(pValue)) {
          long v = PyLong_AsLong(pValue);
          QVariant v1;
          v1.setValue(v);
          ctx->setOutput(symTable, QVariant(v1), sUNDEF, itINTEGER, Resource());
      }else if ( PyFloat_Check(pValue)){
          double v = PyLong_AsLong(pValue);
          ctx->setOutput(symTable, QVariant(v), sUNDEF, itDOUBLE, Resource());
      }else if (PyUnicode_Check(pValue)){
         // PyObject* temp = PyUnicode_AsASCIIString(pValue);
          //char* c_str = PyByteArray_AsString(temp);
          auto str = QString::fromStdString(PyBytes_AsString(PyUnicode_AsUTF8String(const_cast<PyObject*>(pValue))));
          ctx->setOutput(symTable, QVariant(str), sUNDEF, itSTRING, Resource());
      }


      Py_DECREF(pArgs);

      for(auto *p : values)
        Py_DECREF(p);

      Py_XDECREF(pValue);
      Py_XDECREF(pModule);
      Py_XDECREF(pName);

     // Py_Finalize();


    return true;
}

Ilwis::OperationImplementation *CallPythonOperation::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
   return new CallPythonOperation(metaid,expr);
}

Ilwis::OperationImplementation::State CallPythonOperation::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    if ( !_pathsAdded){
        _pathsAdded = true;
        PyRun_SimpleString("import sys\n");
        std::set<QString> seen;
        for(auto kvp : _pythonoperations){
            if (seen.find(kvp.second) == seen.end()){
                seen.insert(kvp.second);
                QFileInfo inf(kvp.second);
                QString path = "sys.path.append(\"" + inf.absolutePath() + "\")";
                PyRun_SimpleString(path.toLatin1());//the folder where the pythonTest.py is located
            }
        }

    }
    QString operName =_expression.name(true);
    auto iter = _pythonoperations.find(operName);
    if ( iter == _pythonoperations.end()){
        kernel()->issues()->log(TR("Couldn't find operation ") + operName);
        return sPREPAREFAILED;
    }
    _pythonFile = (*iter).second;
    if (!_pythonFile.exists()){
        kernel()->issues()->log(TR("Couldn't find python file ") + _pythonFile.absoluteFilePath());
        return sPREPAREFAILED;
    }
    _pythonMetadata = mastercatalog()->name2Resource(operName, itSINGLEOPERATION);
    if ( !_pythonMetadata.isValid()){
        kernel()->issues()->log(TR("Couldn't find operation ") + operName);
        return sPREPAREFAILED;
    }


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

