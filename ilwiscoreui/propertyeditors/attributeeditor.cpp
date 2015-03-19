#include "raster.h"
#include "featurecoverage.h"
#include "feature.h"
#include "table.h"
#include "attributeeditor.h"

VisualAttributeEditor::VisualAttributeEditor(QObject *parent) : QObject(parent)
{

}

VisualAttributeEditor::VisualAttributeEditor(const QString& name, const QString& displayNm, const QUrl &url, QObject *parent) :
    QObject(parent), Ilwis::Identity(name),
    _qmlUrl(url),
    _displayName(displayNm)
{

}

VisualAttributeEditor::VisualAttributeEditor(const VisualAttributeEditor& metadata, QObject *parent) : QObject(parent), Ilwis::Identity(metadata.name())
{
    _qmlUrl = metadata._qmlUrl;
}

QString VisualAttributeEditor::qmlUrl() const
{
    return _qmlUrl.toString();
}

QString VisualAttributeEditor::editorName() const
{
    return name();
}

int VisualAttributeEditor::defaultHeight() const
{
    return 0;
}

bool VisualAttributeEditor::canUse(const IIlwisObject &, const ColumnDefinition &) const
{
    return false;
}

bool VisualAttributeEditor::canUse(const Ilwis::IIlwisObject &, const QString &) const
{
    return false;
}

void VisualAttributeEditor::prepare(CoverageLayerModel *parentLayer, const IIlwisObject &, const ColumnDefinition &)
{
    _layer = parentLayer;
}

int VisualAttributeEditor::layerIndex() const
{
    return _layerIndex;
}

QString VisualAttributeEditor::displayName() const
{
    return _displayName;
}

void VisualAttributeEditor::setlayer(quint32 index, CoverageLayerModel *model){
    _layer = model;
    _layerIndex = index;
}

CoverageLayerModel *VisualAttributeEditor::layer() const
{
    return _layer;
}

CoverageLayerModel *VisualAttributeEditor::layer()
{
    return _layer;
}




