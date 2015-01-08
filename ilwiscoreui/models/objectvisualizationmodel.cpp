#include "kernel.h"
#include "uicontextmodel.h"
#include "visualizationmanager.h"
#include "objectvisualizationmodel.h"

ObjectVisualizationModel::ObjectVisualizationModel()
{
}

ObjectVisualizationModel::ObjectVisualizationModel(const Ilwis::Resource &resource, const QList<PropertyEditorMetaData *> &editors, QObject *obj) : ResourceModel(resource, obj), _propertyEditors(editors)
{

}

QQmlListProperty<PropertyEditorMetaData> ObjectVisualizationModel::propertyEditors()
{
    return QQmlListProperty<PropertyEditorMetaData>(this, _propertyEditors);
}


