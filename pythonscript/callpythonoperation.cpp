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
#include "raster.h"
#include "featurecoverage.h"
#include "table.h"
#include "tranquilizer.h"
#include "callpythonoperation.h"
#include "pythonapi/pythonapi_object.h"
#include "pythonapi/pythonapi_ilwisobject.h"

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

template<class T> bool saveResult(quint64 id,ExecutionContext *ctx, SymbolTable &symTable){
    T obj;
    obj.prepare(id);
    QVariant v1;
    v1.setValue(obj);
    if ( obj.isValid()){
        ctx->setOutput(symTable, QVariant(v1), obj->name(), obj->ilwisType(), obj->resource());
    }else{
        kernel()->issues()->log(TR("No valid object in python operation "));
        return false;
    }
    return true;
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
    if ( pModule == 0){
        kernel()->issues()->log(TR("Python code couldnt be loaded. Is the syntax correct?"));
        return false;
    }

    PyObject *pDict = PyModule_GetDict(pModule);//borowed reference so no DECREF
    if ( pDict == 0)
        return false;

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

    bool ok = true;
    if ( pValue){
        QVariant v1;

        IlwisTypes expectedType = _pythonMetadata["pout_1_type"].toULongLong();
        if ( hasType(expectedType, itILWISOBJECT)){
            quint64 id = PyLong_AsUnsignedLongLong(pValue);
            if (hasType(expectedType, itRASTER)){
                ok = saveResult<IRasterCoverage>(id,ctx, symTable);
            } else if (hasType(expectedType, itFEATURE)){
                ok = saveResult<IFeatureCoverage>(id,ctx, symTable);
            }else if (hasType(expectedType, itTABLE)){
                ok =  saveResult<ITable>(id,ctx, symTable);
            }
        }else if (PyLong_Check(pValue)) {
            long v = PyLong_AsLong(pValue);
            v1.setValue(v);
            ctx->setOutput(symTable, QVariant(v1), sUNDEF, itINTEGER, Resource());

        }else if ( PyFloat_Check(pValue)){
            double v = PyLong_AsLong(pValue);
            ctx->setOutput(symTable, QVariant(v), sUNDEF, itDOUBLE, Resource());
        }else if (PyUnicode_Check(pValue)){
            auto str = QString::fromStdString(PyBytes_AsString(PyUnicode_AsUTF8String(const_cast<PyObject*>(pValue))));
            ctx->setOutput(symTable, QVariant(str), sUNDEF, itSTRING, Resource());
        }else {
            if ( _expression.parameterCount(false) > 0){
                kernel()->issues()->log(TR("Unexpected type returned from python operation") + QString(pValue->ob_type->tp_doc));
                ok = false;
            }
        }
    }


    Py_DECREF(pArgs);

    for(auto *p : values)
        Py_DECREF(p);

    Py_XDECREF(pValue);
    Py_XDECREF(pModule);
    Py_XDECREF(pName);

    // Py_Finalize();


    return ok;
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

