#include "kernel.h"
#include "uicontextmodel.h"
#include "visualizationmanager.h"
#include "objectvisualizationmodel.h"

ObjectVisualizationModel::ObjectVisualizationModel()
{
}

ObjectVisualizationModel::ObjectVisualizationModel(const Ilwis::Resource &resource, QObject *obj) : ResourceModel(resource, obj)
{

}

void ObjectVisualizationModel::setEditors()
{
   std::map<QString, CreatePropertyEditor> editors = uicontext()->propertyEditors(item().ilwisType());
   for(auto creator: editors){
        PropertyEditor *editor = creator.second(this, IOOptions());
        if ( editor)
            _propertyEditors[editor->name()] = editor;
   }

}

