#include "kernel.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "operationmetadata.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationExpression.h"
#include "script.h"
#include "pythonscriptconnector.h"

using namespace Ilwis;
using namespace PythonScript;

PythonScriptConnector::PythonScriptConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) :
        IlwisObjectConnector(resource, load, options)
{

}

bool PythonScriptConnector::loadMetaData(IlwisObject *obj, const Ilwis::IOOptions &options)
{
    if (!obj)
            return false;
    return true;
}

bool PythonScriptConnector::loadData(IlwisObject* obj, const IOOptions& options) {
    QFile file(_resource.url(true).toLocalFile());
    if (file.open(QFile::ReadOnly | QFile::Text)){
        QString text = file.readAll();
        Script *script = static_cast<Script *>(obj);
        script->text(text);
        _binaryIsLoaded = true;
        file.close();
        return true;
    }
    return false;
}

bool PythonScriptConnector::store(IlwisObject *obj, const IOOptions &options)
{
    if (!obj)
            return false;
    Script *script = static_cast<Script *>(obj);
    QFile file(_resource.url(true).toLocalFile());
    if (file.open(QFile::WriteOnly | QFile::Text)){
        file.write(script->text().toLocal8Bit());
        file.close();
        return true;
    }
    return false;
}

QString PythonScriptConnector::type() const
{
    return "Script";
}

Ilwis::IlwisObject *PythonScriptConnector::create() const
{
    return new Script(_resource);
}

ConnectorInterface *PythonScriptConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions &options)
{
  return new PythonScriptConnector(resource, load, options);
}

QString PythonScriptConnector::provider() const
{
  return "python";
}
