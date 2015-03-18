
#include "kernel.h"
#include "workflowcatalogmodel.h"

using namespace Ilwis;



WorkflowCatalogModel::WorkflowCatalogModel(QObject *parent)
{

}

QMLWorkflowList WorkflowCatalogModel::workflows()
{
    try{
        if ( _currentWorkflows.isEmpty()) {
            if ( !_view.isValid())
                return QMLWorkflowList();

            // TODO implement dummy workflows
            //gatherItems();
            gatherDummyItems();

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

void WorkflowCatalogModel::nameFilter(const QString &)
{
    // TODO implement filtering
}

void WorkflowCatalogModel::gatherDummyItems()
{
    Resource dummy1("ilwis://internalcatalog/workflow_addRaster", itOPERATIONMETADATA);
    dummy1.prepare();
    WorkflowModel *model1 = new WorkflowModel(dummy1);

    _currentWorkflows.push_back(model1);
}


