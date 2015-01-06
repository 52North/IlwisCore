#include <QDebug>
#include <memory>
#include "iooptions.h"
#include "visualizationmanager.h"

VisualizationManager::VisualizationManager(QObject *parent) :
    QObject(parent)
{
}
void VisualizationManager::addVisualizationModel(ObjectVisualizationModel *newmodel)
{
    _layers.append(newmodel);
}

ObjectVisualizationModel *VisualizationManager::layer(quint32 layerIndex){
    if ( layerIndex < _layers.size())
        return _layers[layerIndex];
    return 0;
}

void VisualizationManager::init()
{

}
