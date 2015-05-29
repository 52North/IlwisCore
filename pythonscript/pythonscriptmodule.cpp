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

#include "pythonscript/pythonscriptmodule.h"

using namespace Ilwis;
using namespace PythonScript;

PythonScriptModule::PythonScriptModule(QObject *parent) : Module(parent, "PythonScriptModule", "iv40","1.0")
{

}

PythonScriptModule::~PythonScriptModule()
{
    Py_Finalize();
}

QString PythonScriptModule::getInterfaceVersion() const
{
      return "iv40";
}

QString PythonScriptModule::getName() const
{
    return "pythonscript plugin";
}

QString PythonScriptModule::getVersion() const
{
    return "1.0";
}

void PythonScriptModule::prepare()
{
    try{
        QString home = ilwisconfig("extensionsettings/pythonscript/python-home", QString(sUNDEF));
        if ( home == sUNDEF){
            home = QString::fromWCharArray(Py_GetPythonHome());
        }
        QString checkhome = home + "/DLLs/python3.dll";
        QFileInfo inf(checkhome);
        if ( !inf.exists()){
            ERROR2(ERR_COULD_NOT_LOAD_2,"Python instance","Python scripting unavailable");
            return;
        }
        wchar_t data[400];
        std::memset(data, 0, 400);
        if ( home.toWCharArray(data) > 0){
            Py_SetPythonHome(data);
            Py_Initialize();
        }else {
            ERROR2(ERR_COULD_NOT_LOAD_2,"Python instance","Python scripting unavailable");
            return;
        }
          commandhandler()->addOperation(RunPython::createMetadata(), RunPython::create);

        kernel()->issues()->log("Loaded python script module",IssueObject::itMessage);
    } catch(std::exception ex){
        qDebug() << ex.what();
    }



}
