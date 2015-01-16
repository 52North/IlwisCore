#include "kernel.h"
#include "uicontextmodel.h"
#include "visualizationmanager.h"
#include "coveragelayermodel.h"

CoverageLayerModel::CoverageLayerModel()
{
}

CoverageLayerModel::CoverageLayerModel(const Ilwis::Resource &resource,
                                       const QList<PropertyEditor *> &editors,
                                       Ilwis::Geodrawer::DrawerInterface *drawer,QObject *obj) :
    ResourceModel(resource, obj), _propertyEditors(editors), _drawer(drawer)
{
    for(auto editor : _propertyEditors){
        editor->setParent(this);
        editor->layer(this);
    }
}

PropertyEditor *CoverageLayerModel::propertyEditor(const QString &name)
{
    for(auto editor : _propertyEditors){
        if ( editor->editorName() == name)
            return editor;
    }
    return 0;
}

QQmlListProperty<PropertyEditor> CoverageLayerModel::propertyEditors()
{
    return QQmlListProperty<PropertyEditor>(this, _propertyEditors);
}


