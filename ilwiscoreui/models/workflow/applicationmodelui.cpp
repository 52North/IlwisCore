#include "kernel.h"
#include "ilwisdata.h"
#include "workflow.h"
#include "analysispattern.h"
#include "applicationsetup.h"
#include "model.h"
#include "applicationmodel.h"

ApplicationModel::ApplicationModel(Ilwis::ModelApplication *app, QObject *parent) : QObject(parent), _app(app)
{

}

QString ApplicationModel::name() const
{
    if ( _app)    {
       return _app->name();
    }
    return "";
}

QString ApplicationModel::attachedAnalysis() const
{
    if ( _app)
        return _app->attachedAnalysis();
    return "";
}

void ApplicationModel::attachedAnalysis(const QString &name)
{
    if ( _app)
        _app->attachedAnalysis(name);
}

QString ApplicationModel::panel(const QString &panelName)
{
    auto iter = _panels.find(panelName);
    if ( iter != _panels.end()){
        return (*iter).second;
    }
    return "";
}
