#include "kernel.h"
#include "ilwisdata.h"
#include "modeller/workflow.h"
#include "modeller/applicationmodel.h"
#include "modeller/analysispattern.h"
#include "modellerfactory.h"

using namespace Ilwis;

ModellerFactory::ModellerFactory()  : AbstractFactory("ModellerFactory","ilwis","Creates the various application, workflows and analyses used in a model")
{

}

AnalysisPattern *ModellerFactory::createAnalysisPattern(const QString type, const QString &name, const QString &description, const IOOptions &options)
{
    auto iter = _analysisCreators.find(type.toLower());
    if ( iter == _analysisCreators.end()){
        return 0;
    }
    return (*iter).second(name, description, options);
}

ApplicationModel *ModellerFactory::createApplication(const QString type)
{
    auto iter = _applicationCreators.find(type.toLower());
    if ( iter == _applicationCreators.end()){
        return 0;
    }
    return (*iter).second();
}

QStringList ModellerFactory::analysisTypes() const
{
    QStringList result;
    for(auto analysis : _analysisCreators){
        result.push_back(analysis.first);
    }
    return result;
}

QStringList ModellerFactory::applications(const QString &analysisType)
{
    QStringList result;
    for(auto creator : _applicationCreators){
        ApplicationModel * amodel = creator.second();
        if ( amodel->attachedAnalysis() == analysisType){
            result.push_back(amodel->name());
        }
        delete amodel;
    }

    return result;
}

AnalysisPattern *ModellerFactory::registerAnalysisPattern(const QString &classname, CreateAnalysisPattern createFunc)
{
    ModellerFactory *factory = kernel()->factory<ModellerFactory>("ModellerFactory", "ilwis");
    if ( factory){
        factory->registerAnalysisPatternInternal(classname, createFunc);
    }
    return 0;
}

ApplicationModel *ModellerFactory::registerModelApplication(const QString &classname, CreateModelApplication createFunc)
{
    ModellerFactory *factory = kernel()->factory<ModellerFactory>("ModellerFactory", "ilwis");
    if ( factory){
        factory->registerModelApplicationInternal(classname, createFunc);
    }
    return 0;
}

void ModellerFactory::registerAnalysisPatternInternal(const QString &classname, CreateAnalysisPattern createFunc)
{
    auto iter = _analysisCreators.find(classname.toLower());
    if ( iter == _analysisCreators.end()){
        _analysisCreators[classname.toLower()] = createFunc;
    }

}

void ModellerFactory::registerModelApplicationInternal(const QString &classname, CreateModelApplication createFunc)
{
    auto iter = _applicationCreators.find(classname.toLower());
    if ( iter == _applicationCreators.end()){
        _applicationCreators[classname.toLower()] = createFunc;
    }
}
