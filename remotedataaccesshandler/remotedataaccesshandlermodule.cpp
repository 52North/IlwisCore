#include <QtPlugin>
#include "kernel.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "symboltable.h"
#include "remotedataaccesshandlermodule.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "catalog.h"
#include "remotedataaccessrequesthandler.h"
#include "remoteoperationrequesthandler.h"
#include "httpserver/requesthandlerfactory.h"




using namespace Ilwis;
using namespace RemoteDataAccess;

RemoteDataAccessHandlerModule::RemoteDataAccessHandlerModule(QObject *parent) :
    Module(parent, "RemoteDataAccessHandlerModule", "iv40","1.0")
{
}

QString RemoteDataAccessHandlerModule::getInterfaceVersion() const
{
    return "iv40";

}

QString RemoteDataAccessHandlerModule::getName() const
{
    return "RemoteDataAccessHandler plugin";
}

QString RemoteDataAccessHandlerModule::getVersion() const
{
    return "1.0";
}

void RemoteDataAccessHandlerModule::finalizePreparation()
{
    HTTP::HTTPRequestHandlerFactory *factory = kernel()->factory<HTTP::HTTPRequestHandlerFactory>("ilwis::HTTPRequestHandlerFactory");
    if ( factory){
        factory->addHandler("/dataaccess", RemoteDataAccessRequestHandler::create);
        factory->addHandler("/operation", RemoteOperationRequestHandler::create);
    }
}

void RemoteDataAccessHandlerModule::prepare()
{

  kernel()->issues()->log("Loaded remote data access module",IssueObject::itMessage);

}





