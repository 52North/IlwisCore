#include "propertyeditor.h"

PropertyEditor::PropertyEditor(QObject *parent) : QObject(parent)
{

}

PropertyEditor::PropertyEditor(const QString& name, const QString& displayNm, const QUrl &url, QObject *parent) :
    QObject(parent), Ilwis::Identity(name),
    _qmlUrl(url),
    _displayName(displayNm)
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

int PropertyEditor::defaultHeight() const
{
    return 0;
}

bool PropertyEditor::canUse(const IIlwisObject &) const
{
    return false;
}

void PropertyEditor::prepare(const IIlwisObject &)
{

}

int PropertyEditor::layerIndex() const
{
    return _layerIndex;
}

QString PropertyEditor::displayName() const
{
    return _displayName;
}

void PropertyEditor::setlayer(quint32 index, CoverageLayerModel *model){
    _layer = model;
    _layerIndex = index;
}

CoverageLayerModel *PropertyEditor::layer() const
{
    return _layer;
}

CoverageLayerModel *PropertyEditor::layer()
{
    return _layer;
}



