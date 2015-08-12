#include <future>
#include <QUuid>

#include "kernel.h"
#include "ilwistypes.h"
#include "raster.h"
#include "models/resourcemodel.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "models/catalogmodel.h"
#include "workflow.h"

#include "workflowmodel.h"
#include "workflowcatalogmodel.h"

using namespace Ilwis;

WorkflowCatalogModel::WorkflowCatalogModel(QObject *parent): CatalogModel()
{

    // TODO create a dummy list
    newWorkflow("workflow_dummy1", "super artificial description text");

    // creates workflow catalog
    QUrl location("ilwis://operations");
    QString descr ="main catalog for ilwis workflows";
    Resource res(location, itCATALOGVIEW ) ;
    res.name("ilwis-workflows",false);
    QStringList lst;
    lst << location.toString();
    res.addProperty("locations", lst);
    res.addProperty("type", "workflow" );
    res.addProperty("filter",QString("type=%1").arg(itWORKFLOW));
    res.setDescription(descr);
    setView(CatalogView(res));
}

WorkflowModel *WorkflowCatalogModel::newWorkflow(const QString &name, const QString &description)
{
    QUrl url(QString("ilwis://operations/workflow_%1").arg(name));
    OperationResource opResource( url );
    opResource.setIlwisType(itWORKFLOW);

    if (mastercatalog()->contains(url, itWORKFLOW)) {
        error("Workflow with name " + name + " already exists.");
        return new WorkflowModel();
    }

    opResource.setLongName(name);
    opResource.setDescription(description);
    mastercatalog()->addItems({opResource});

    IWorkflow flow;
    if (!flow.prepare(opResource)) {
        ERROR1("Could not initialize workflow with name '%1'", name);
        error("Could not initialize workflow.");
        return new WorkflowModel();
    }

    // TODO no workflow connector available (yet?) which could store the resource
    Resource resource = mastercatalog()->id2Resource(opResource.id());
    WorkflowModel *model = new WorkflowModel(resource);

    //WorkflowModel *model = new WorkflowModel(opResource);
    return model; // temporary until it is saved
}

bool WorkflowCatalogModel::deleteWorkflow(quint32 index)
{
    // TODO add delete method via mastercatalog
}

WorkflowModel *WorkflowCatalogModel::saveWorkflow(quint32 index)
{
    // TODO save workflow via mastercatalog
}

QString WorkflowCatalogModel::executeworkflow(quint64 workflowid, const QString &parameters)
{
    // TODO execute workflow on a datastructure ... we have to enable debug flags


    if ( workflowid == 0 || parameters == "")
        return sUNDEF;

    Resource operationresource = mastercatalog()->id2Resource(workflowid);
    if ( !operationresource.isValid())
        return sUNDEF;

    QString expression;
    QStringList parms = parameters.split("|");

    for(int i = 0; i < parms.size() - 1; ++ i){ // -1 because the last is the output parameter
        if ( expression.size() != 0)
            expression += ",";
        expression += parms[i];
    }
    QString output = parms[parms.size() - 1];
    expression = QString("%1=%2(%3)").arg(output).arg(operationresource.name()).arg(expression);

    OperationExpression opExpr(expression);

    try {

        qDebug() << "TODO executing";

        //std::async(std::launch::async, startWorkflow, opExpr, this);

        return "TODO";
    } catch (const ErrorObject& err){
        emit error(err.message());
    }
    return sUNDEF;

}

QMLWorkflowList WorkflowCatalogModel::workflows()
{
    try {
        if ( _currentWorkflows.isEmpty()) {
            if ( !_view.isValid()) {
                qDebug() << "invalid catalog view. return empty list";
                return QMLWorkflowList();
            }

            gatherItems();
            _currentWorkflows.clear();

            for(auto item : _currentItems){
                QString keywords = item->resource()["keyword"].toString();
                if ( keywords.indexOf("internal") != -1)
                    continue;
                _currentWorkflows.push_back(new WorkflowModel(item->resource(), this));
            }

        }

        return  QMLWorkflowList(this, _currentWorkflows);
    }
    catch(const ErrorObject& err){

    }
    return  QMLWorkflowList();
}

void WorkflowCatalogModel::nameFilter(const QString &filter)
{
    QString workflowFilter(filter);
    workflowFilter.append(" and keyword='workflow'");
    CatalogModel::nameFilter(workflowFilter);
    _currentWorkflows.clear();
    emit workflowsChanged();
}

QString WorkflowCatalogModel::currentWorkflow() const
{
    return _currentWorkflow;
}

void WorkflowCatalogModel::setCurrentWorkflow(const QString &workflowName)
{
    _currentWorkflow = workflowName;
    currentWorkflowChanged();
}

bool WorkflowCatalogModel::hasActiveEditSession() const
{
    return !_currentWorkflow.isEmpty();
}

quint64 WorkflowCatalogModel::workflowId(quint32 index) const
{
    if ( index < _currentWorkflows.size()){
            return _currentWorkflows[index]->id().toULongLong();
        }
        return i64UNDEF;
}
