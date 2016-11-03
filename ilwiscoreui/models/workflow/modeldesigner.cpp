#include "kernel.h"
#include "ilwisdata.h"
#include "mastercatalog.h"
#include "workflow.h"
#include "operationmetadata.h"
#include "workflowmodel.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationcatalogmodel.h"
#include "../workflowerrormodel.h"
#include "ilwiscontext.h"
#include "ilwistypes.h"
#include "applicationsetup.h"
#include "analysispattern.h"
#include "modeldesigner.h"

ModelDesigner::ModelDesigner(QObject *parent) : QObject(parent)
{
    if ( parent)
        _model.prepare();
}

ModelDesigner::ModelDesigner(IModel& model, QObject *parent) : QObject(parent)
{
    if ( model.isValid()){
        _model = model;
        for(int i=0; i < _model->workflowCount(); ++i){
            _workflowmodels.push_back(new WorkflowModel(_model->workflow(i)->resource(), this));
        }
    }
}

qint32 ModelDesigner::workflowCount() const
{
    return _workflowmodels.size();
}

WorkflowModel *ModelDesigner::workflow(qint32 index) const
{
    if ( index < _workflowmodels.size())
        return _workflowmodels[index];
    return 0;
}

WorkflowModel *ModelDesigner::workflow(const QString &name) const
{
    for(WorkflowModel *wf : _workflowmodels ){
        if ( wf->name() == name){
            return wf;
        }
    }
    return 0;
}

WorkflowModel *ModelDesigner::addWorkflow(const QString& filter)
{
    try {
        if (_model.isValid()){
            std::vector<Resource> res = Ilwis::mastercatalog()->select(filter);
            if ( res.size() == 1){
                IWorkflow wf;
                wf.prepare(res[0]);
                if (wf.isValid() && _model->addWorklfow(wf)){
                    auto *wfm = new WorkflowModel(res[0], this);
                    _workflowmodels.push_back(wfm);
                    emit workflowCountChanged();
                    return wfm;
                }
                Ilwis::kernel()->issues()->log(QString(TR("workflow couldnt be added, maybe a duplicate?")).arg(filter),Ilwis::IssueObject::itWarning);
                return 0;
            }else {

                Ilwis::kernel()->issues()->log(QString(TR("workflow %1 doesnt exist")).arg(filter),Ilwis::IssueObject::itWarning);
            }
        }
    }
    catch(const ErrorObject& err){}

    return 0;
}

void ModelDesigner::removeWorkflow(const QString &name)
{
    for(int i =0; i < _workflowmodels.size(); ++i ){
        if ( _workflowmodels[i]->name() == name){
            _workflowmodels.erase(_workflowmodels.begin() + i);
            break;
        }
    }
}

void ModelDesigner::removeWorkflow(qint32 index)
{
    if ( index < _workflowmodels.size())
        _workflowmodels.erase(_workflowmodels.begin() + index);
}

QStringList ModelDesigner::workflowNames() const{
    QStringList names;
    for(WorkflowModel *wf : _workflowmodels )    {
        names.push_back(wf->name());
    }
    return names;
}

QStringList ModelDesigner::applicationNames() const
{
    return QStringList();
}

QStringList ModelDesigner::analysisNames() const
{
    QStringList names;
    for(AnalysisModel *am : _analysismodels )    {
        names.push_back(am->name());
    }
    return names;
}

QStringList ModelDesigner::conceptNames() const
{
    return QStringList();
}

qint32 ModelDesigner::analysisCount() const
{
    return _analysismodels.size();
}

void ModelDesigner::removeAnalysisPattern(const QString &name)
{
    for(int i =0; i < _analysismodels.size(); ++i ){
        if ( _analysismodels[i]->name() == name){
            _analysismodels.erase(_analysismodels.begin() + i);
            break;
        }
    }
}

void ModelDesigner::removeAnalysisPattern(qint32 index)
{
    if ( index < _analysismodels.size())
        _analysismodels.erase(_analysismodels.begin() + index);
}

qint32 ModelDesigner::applicationCount() const
{
    return _appmodels.size();
}

void ModelDesigner::removeApplication(const QString &name)
{

}

void ModelDesigner::removeApplication(qint32 index)
{

}

qint32 ModelDesigner::conceptCount() const
{
    return _conceptmodels.size();
}
