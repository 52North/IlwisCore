#include "kernel.h"

#include "uicontextmodel.h"

quint64 UIContextModel::_objectCounter = 0;

UIContextModel::UIContextModel(QObject *parent) :
    QObject(parent)
{
}

VisualizationManager *UIContextModel::createVisualizationManager(const QString& objectName)
{
    QObject *object =_qmlcontext->findChild<QObject *>(objectName);
    QObject *newparent = object == 0 ? this : object;
    VisualizationManager *manager =  new VisualizationManager(newparent, this);

    return manager;
}

QString UIContextModel::uniqueName()
{
    return "ilwis_ui_object_" + QString::number(_objectCounter++);
}

QList<PropertyEditorMetaData *> UIContextModel::propertyEditors(quint64 objecttype)
{
    QList<PropertyEditorMetaData *> editors;
    auto iter = _propertyEditors.find(objecttype);
    if ( iter != _propertyEditors.end()){
        for(const auto &editor : (*iter).second)    {
            editors.append(editor.second);
        }
    }
    return editors;
}

void UIContextModel::qmlContext(QQmlContext *ctx)
{
    _qmlcontext = ctx;
}

void UIContextModel::addPropertyEditor(quint64 objecttype, const QString &propertyName, const PropertyEditorMetaData& metadata)
{
    _propertyEditors[objecttype][propertyName] = new PropertyEditorMetaData(metadata, this);
}


