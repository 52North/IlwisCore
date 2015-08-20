#include "kernel.h"
#include "ilwisdata.h"
#include "workflowmodel.h"
#include "scenariodesignermodel.h"
#include "mastercatalog.h"

using namespace Ilwis;

ScenarioDesignerModel::ScenarioDesignerModel(QObject *parent) : QObject(parent)
{

}

ScenarioDesignerModel::~ScenarioDesignerModel()
{

}

WorkflowModel *ScenarioDesignerModel::workflowModel(const QString &idname)
{
    for(auto *wf : _workflows)    {
        if ( wf->name() == idname){
            return wf;
        }
    }
    return 0;
}

WorkflowModel *ScenarioDesignerModel::addWorkflow(const QString& source)
{
    std::vector<Resource> res = Ilwis::mastercatalog()->select(source);
    if ( res.size() == 1){
        auto *wf = new WorkflowModel(res[0], this);
        _workflows.append(wf);
        return wf;
    }else {
        Ilwis::kernel()->issues()->log(QString(TR("workflow %1 doesnt exist")).arg(source),Ilwis::IssueObject::itWarning);
    }
    return 0;
}

