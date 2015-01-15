#include "propertyeditor.h"

PropertyEditor::PropertyEditor(QObject *parent) : QObject(parent)
{

}

PropertyEditor::PropertyEditor(const QString& name, const QUrl &url, QObject *parent) : QObject(parent), Ilwis::Identity(name), _qmlUrl(url)
{

}

PropertyEditor::PropertyEditor(const PropertyEditor& metadata, QObject *parent) : QObject(parent), Ilwis::Identity(metadata.name())
{
    _qmlUrl = metadata._qmlUrl;
}

QString PropertyEditor::qmlUrl() const
{
    return _qmlUrl.toString();
}

QString PropertyEditor::editorName() const
{
    return name();
}

void PropertyEditor::layer(CoverageLayerModel *model){
    _layer = model;
}

CoverageLayerModel *PropertyEditor::layer()
{
    return _layer;
}



