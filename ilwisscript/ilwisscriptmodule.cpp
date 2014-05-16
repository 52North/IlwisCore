#include <QtPlugin>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include "kernel.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "ilwisscriptmodule.h"
#include "ilwisdata.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "script.h"

using namespace Ilwis;

IlwisScriptModule::IlwisScriptModule(QObject *parent) :
    Module(parent, "IlwisScript", "iv40","1.0")
{
}

QString IlwisScriptModule::getInterfaceVersion() const
{
    return "iv40";

}

void IlwisScriptModule::prepare()
{
    commandhandler()->addOperation(Script::createMetadata(), Script::create);

}

QString IlwisScriptModule::name() const
{
    return "IlwisScript plugin";
}

QString IlwisScriptModule::version() const
{
    return "1.0";
}






