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
#include "resourcemodel.h"
#include "applicationmodel.h"
#include "analysispattern.h"
#include "modeldesigner.h"
#include "modellerfactory.h"
#include "modelbuilder.h"

std::unique_ptr<ModelBuilder> ModelBuilder::_modelbuilder;

using namespace Ilwis;

ModelBuilder::ModelBuilder(QObject *parent) : QObject(parent)
{

}

ModelDesigner *ModelBuilder::createModel(ResourceModel *rmodel,QObject *parent)
{
    try {
        return new ModelDesigner(rmodel, parent);
    }
    catch( const ErrorObject& ){}

    return 0;
}

AnalysisModel *ModelBuilder::createAnalysisModel(AnalysisPattern *pattern)
{
    auto iter = _analysisModelCreators.find(pattern->type());
    if ( iter != _analysisModelCreators.end()){
         return (*iter).second(pattern);
    }
    return 0;
}

ApplicationModelUI * ModelBuilder::createApplicationModelUI(ApplicationModel *appmodel, QObject *parent){
    for(auto creator : _applicationModelCreators){
        ApplicationModelUI *appmodelui = creator.second(appmodel, parent);
        if ( appmodel->name() == appmodelui->name()){
            return appmodelui;
        }
        delete appmodelui;
    }
    return 0;
}

AnalysisModel *ModelBuilder::registerAnalysisModel(const QString &type, CreateAnalysisModel creator)
{
    auto iter = _analysisModelCreators.find(type);
    if ( iter == _analysisModelCreators.end()){
        _analysisModelCreators[type] = creator;
    }
    return 0;
}

ApplicationModelUI *ModelBuilder::registerApplicationModel(const QString &type, CreateAppModel creator)
{
    auto iter = _applicationModelCreators.find(type);
    if ( iter == _applicationModelCreators.end()){
        _applicationModelCreators[type] = creator;
    }
    return 0;
}
