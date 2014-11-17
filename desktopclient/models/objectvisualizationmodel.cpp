#include "kernel.h"
#include "visualizationmanager.h"
#include "objectvisualizationmodel.h"

ObjectVisualizationModel::ObjectVisualizationModel()
{
}

ObjectVisualizationModel::ObjectVisualizationModel(const Ilwis::Resource &resource, QObject *obj) : ResourceModel(resource, obj)
{

}

void ObjectVisualizationModel::setEditors(const VisualizationManager &manager)
{
    _propertyEditor = manager.propertyEditors(item().ilwisType());

}

QList<QString> ObjectVisualizationModel::properties() const {
    QList<QString> properties;
    for(auto property : _propertyEditor)    {
        properties.append(property.second);
    }
    return properties;
}
