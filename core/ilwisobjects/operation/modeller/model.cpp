#include "kernel.h"
#include "ilwisdata.h"
#include "operationmetadata.h"
#include "workflow.h"
#include "analysispattern.h"
#include "applicationsetup.h"
#include "model.h"

using namespace Ilwis;

Model::Model()
{

}

Model::Model(const Resource &resource) : IlwisObject(resource)
{

}

IlwisTypes Model::ilwisType() const
{
    return itMODEL;
}

qint32 Model::workflowCount() const
{
    return _workflows.size();
}

qint32 Model::analysisCount() const
{
    return _analysisPatterns.size();
}

IWorkflow Model::workflow(qint32 index) const
{
    if ( index < _workflows.size())
        return _workflows[index];
    return IWorkflow();
}

IWorkflow Model::workflow(const QString &name) const
{
    for(IWorkflow wf : _workflows){
        if ( wf->name() == name)
            return wf;
    }
    return IWorkflow();
}

bool Model::addWorklfow(const IWorkflow& wf)
{
      for(const IWorkflow& wfExist : _workflows){
          if ( wfExist->name() == wf->name()){
              kernel()->issues()->log(TR("No duplicate names allowed as workflow name:") + wf->name());
              return false;
          }
      }
      _workflows.push_back(wf);
      return true;
}

void Model::removeWorkflow(const QString &name)
{
    for(int i=0; i < _workflows.size(); ++i){
        if ( _workflows[i]->name() == name){
            _workflows.erase(_workflows.begin() + i);
            return;
        }
    }
}

void Model::removeWorkflow(qint32 index)
{
    for(int i=0; i < _workflows.size(); ++i){
        if ( i == index){
            _workflows.erase(_workflows.begin() + i);
            return;
        }
    }
}

SPAnalysisPattern Model::analysisPattern(qint32 index) const
{
    if ( index < _analysisPatterns.size())
        return _analysisPatterns[index];
    return SPAnalysisPattern();
}

SPAnalysisPattern Model::analysisPattern(const QString &name) const
{
    for(SPAnalysisPattern pattern : _analysisPatterns){
        if ( pattern->name() == name)
            return pattern;
    }
    return SPAnalysisPattern();
}

bool Model::addAnalysisPattern(AnalysisPattern *pattern)
{
      for(SPAnalysisPattern existingPattern : _analysisPatterns){
          if ( existingPattern->name() == pattern->name()){
              kernel()->issues()->log(TR("No duplicate names allowed as Analysis name:") + pattern->name());
              return false;
          }
      }
      pattern->attachedModel(id());
      _analysisPatterns.push_back(SPAnalysisPattern(pattern));
      return true;
}

void Model::removeAnalysisPattern(const QString &name)
{
    for(int i=0; i < _analysisPatterns.size(); ++i){
        if ( _analysisPatterns[i]->name() == name){
            _analysisPatterns.erase(_analysisPatterns.begin() + i);
            return;
        }
    }
}

void Model::removeAnalysisPattern(qint32 index)
{
    for(int i=0; i < _analysisPatterns.size(); ++i){
        if ( i == index){
            _analysisPatterns.erase(_analysisPatterns.begin() + i);
            return;
        }
    }
}

qint32 Model::applicationCount() const
{
    return _applications.size();
}

SPModelApplication Model::application(qint32 index) const
{
    if ( index < _applications.size())
        return _applications[index];
    return SPModelApplication();
}

SPModelApplication Model::application(const QString &name) const
{
    for(SPModelApplication app : _applications){
        if ( app->name() == name)
            return app;
    }
    return SPModelApplication();
}

bool Model::addApplication(ModelApplication *app)
{
      for(SPModelApplication app : _applications){
          if ( app->name() == app->name()){
              kernel()->issues()->log(TR("No duplicate names allowed as Analysis name:") + app->name());
              return false;
          }
      }
      _applications.push_back(SPModelApplication(app));
      return true;
}

void Model::removeApplication(const QString &name)
{
    for(int i=0; i < _applications.size(); ++i){
        if ( _applications[i]->name() == name){
            _applications.erase(_applications.begin() + i);
            return;
        }
    }
}

void Model::removeApplication(qint32 index)
{
    for(int i=0; i < _applications.size(); ++i){
        if ( i == index){
            _applications.erase(_applications.begin() + i);
            return;
        }
    }
}
