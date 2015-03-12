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

bool VisualAttributeEditor::canUse(const IIlwisObject &) const
{
    return false;
}

void VisualAttributeEditor::prepare(const IIlwisObject &obj)
{
    if ( !hasType(obj->ilwisType(), itCOVERAGE)){
        return ;
    }
    if ( hasType(obj->ilwisType(), itFEATURE)){
        IFeatureCoverage features = obj.as<FeatureCoverage>();
        for(int i =0; i < features->attributeDefinitions().definitionCount(); ++i){
            _attributes.push_back(features->attributeDefinitions().columndefinition(i).name());
        }
    }
    if ( hasType(obj->ilwisType(), itRASTER)){
        IRasterCoverage raster = obj.as<RasterCoverage>();
        if ( raster->hasAttributes()){
            for(int i=0; i < raster->attributeTable()->columnCount(); ++i){
                _attributes.push_back(raster->attributeTable()->columndefinition(i).name());
            }
        }
    }
}

int VisualAttributeEditor::layerIndex() const
{
    return _layerIndex;
}

QString VisualAttributeEditor::displayName() const
{
    return _displayName;
}

bool VisualAttributeEditor::attributeDependent() const
{
    bool res = _attributeDependent && _attributes.size() > 0;
    return res;
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

void VisualAttributeEditor::setAttributeDependent(bool yesno)
{
    _attributeDependent = yesno;
}

QStringList VisualAttributeEditor::attributes() const
{
    return _attributes;
}



