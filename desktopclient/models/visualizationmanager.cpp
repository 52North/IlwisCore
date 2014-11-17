#include <QDebug>
#include <memory>
#include "visualizationmanager.h"

std::unique_ptr<VisualizationManager> VisualizationManager::_manager;

std::unique_ptr<VisualizationManager>& visualizationManager() {
    if ( VisualizationManager::_manager.get() == 0) {
        VisualizationManager::_manager.reset( new VisualizationManager());
        VisualizationManager::_manager->init();

    }
    return VisualizationManager::_manager;
}


VisualizationManager::VisualizationManager(QObject *parent) :
    QObject(parent)
{
}

std::map<QString, QString> VisualizationManager::propertyEditors(quint64 objecttype) const
{
    auto iter = _propertyEditors.find(objecttype);
    if ( iter != _propertyEditors.end()){
        return (*iter).second;
    }
    return std::map<QString, QString>();
}

QMLObjectVisualizationList VisualizationManager::layersOf(quint32 visualizationid){
    auto iter = _visualizations.find(visualizationid);

    if ( iter != _visualizations.end()){
        return QMLObjectVisualizationList(this, iter->second);
    }
    return QMLObjectVisualizationList(this, _empty);
}

void VisualizationManager::addPropertyEditor( quint64 objecttype, const QString &propertyName, const QString &qmlLink)
{
    auto iter = _propertyEditors.find(objecttype);
    if ( iter == _propertyEditors.end()){
            _propertyEditors[objecttype][propertyName] = qmlLink;
    }
}

void VisualizationManager::addVisualizationModel(quint32 visualizatonid, quint32 layerIndex, ObjectVisualizationModel *newmodel)
{

    auto iter =  _visualizations.find(visualizatonid);
    if ( iter == _visualizations.end()){
        QList<ObjectVisualizationModel *> models;
        models.append(newmodel);
        _visualizations[visualizatonid] = models;
    }else {
        if ( layerIndex < (*iter).second.size()){
            (*iter).second[layerIndex] = newmodel;
        }else{
            (*iter).second.append(newmodel);
        }
    }
    qDebug() << _visualizations.size();
}

QStringList VisualizationManager::globalProperties() const
{
    QStringList globals;
    globals.append("3D");
    globals.append("Distance Measurer");
    globals.append("Geometry");
    globals.append("Grid");
    globals.append("Gratticule");

    return globals;

}

ObjectVisualizationModel *VisualizationManager::layer(quint32 visualizationid, quint32 layerIndex){
    auto iter =  _visualizations.find(visualizationid);
    if ( iter != _visualizations.end()){
        if ( layerIndex < (*iter).second.size())
            return (*iter).second[layerIndex];
    }
    return 0;
}

void VisualizationManager::init()
{

}
