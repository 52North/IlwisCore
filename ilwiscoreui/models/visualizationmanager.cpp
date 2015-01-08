#include <QDebug>
#include <memory>
#include "kernel.h"
#include "ilwisdata.h"
#include "mastercatalog.h"
#include "iooptions.h"
#include "uicontextmodel.h"
#include "objectvisualizationmodel.h"
#include "visualizationmanager.h"

using namespace Ilwis;

VisualizationManager::VisualizationManager(QObject *parent) :
    QObject(parent)
{
}

VisualizationManager::VisualizationManager(QObject *parent, UIContextModel *context) : QObject(parent), _uicontext(context)
{

}
void VisualizationManager::addVisualizationModel(ObjectVisualizationModel *newmodel)
{
    _layers.append(newmodel);
}

void VisualizationManager::addDataSource(const QString &url, const QString &typeName)
{
    IlwisTypes tp = IlwisObject::name2Type(typeName);
    if ( tp == itUNKNOWN)
        return;
    Resource resource = mastercatalog()->name2Resource(url,tp);
    if ( !resource.isValid())
        return;

    _layers.append( new ObjectVisualizationModel(resource, _uicontext->propertyEditors(tp), this));
}

QQmlListProperty<ObjectVisualizationModel> VisualizationManager::layers()
{
    return QQmlListProperty<ObjectVisualizationModel>(this, _layers);
}

ObjectVisualizationModel *VisualizationManager::layer(quint32 layerIndex){
    if ( layerIndex < _layers.size())
        return _layers[layerIndex];
    return 0;
}

void VisualizationManager::init()
{

}
