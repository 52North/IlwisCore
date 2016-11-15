#include "kernel.h"
#include "ilwisdata.h"
#include "workflow.h"
#include "analysispattern.h"
#include "applicationsetup.h"
#include "model.h"
#include "applicationmodel.h"

ApplicationModel::ApplicationModel(QObject *parent) : QObject(parent)
{

}

QString ApplicationModel::name() const
{
    if ( _app)    {
       return _app->name();
    }
    return "";
}
