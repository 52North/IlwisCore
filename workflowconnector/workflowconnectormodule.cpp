#include "kernel.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "connectorfactory.h"
#include "catalogconnectorfactory.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "workflowconnectorobjectfactory.h"
#include "connectorinterface.h"
#include "ilwiscontext.h"
//#include "ilwisdata.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationmetadata.h"
#include "operationExpression.h"
#include "operation.h"
#include "jsonconfig.h"
#include "modeller/workflownode.h"
#include "modeller/workflow.h"
#include "workflowconnectormodule.h"
#include "workflowjsonconnector.h"

using namespace Ilwis;
using namespace WorkflowConnector;

WorkflowConnectorModule::WorkflowConnectorModule(QObject *parent) : Module(parent, "WorkflowConnectorModule", "iv40","1.0")
{

}

QString WorkflowConnectorModule::getInterfaceVersion() const
{
      return "iv40";
}

QString WorkflowConnectorModule::getName() const
{
    return "JSonWorkflow plugin";
}

QString WorkflowConnectorModule::getVersion() const
{
    return "1.0";
}

void WorkflowConnectorModule::finalizePreparation()
{

}

void WorkflowConnectorModule::prepare()
{
    WorkflowConnectorObjectFactory *factory = new WorkflowConnectorObjectFactory();
    factory->prepare();
    kernel()->addFactory(factory );

    ConnectorFactory *cfactory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!cfactory)
        return;

    cfactory->addCreator(itWORKFLOW, "Json", WorkflowJSONConnector::create);
    cfactory->addCreator("workflow", "Json", WorkflowJSONConnector::create);

    kernel()->issues()->log("Loaded Workflow module",IssueObject::itMessage);

}
