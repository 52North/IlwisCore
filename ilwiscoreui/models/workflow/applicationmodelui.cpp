#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "operationmetadata.h"
#include "workflownode.h"
#include "workflow.h"
#include "analysispattern.h"
#include "applicationmodel.h"
#include "model.h"
#include "applicationmodelui.h"

ApplicationModelUI::ApplicationModelUI(QObject *parent) : QObject(parent)
{

}

ApplicationModelUI::ApplicationModelUI(Ilwis::ApplicationModel *app, QObject *parent) : QObject(parent), _app(app)
{

}

QString ApplicationModelUI::name() const
{
    if ( _app)    {
       return _app->name();
    }
    return "";
}

QString ApplicationModelUI::attachedAnalysis() const
{
    if ( _app)
        return _app->attachedAnalysis();
    return "";
}

void ApplicationModelUI::attachedAnalysis(const QString &name)
{
    if ( _app)
        _app->attachedAnalysis(name);
}

QString ApplicationModelUI::panel(const QString &panelName)
{
    auto iter = _panels.find(panelName);
    if ( iter != _panels.end()){
        return (*iter).second;
    }
    return "";
}
