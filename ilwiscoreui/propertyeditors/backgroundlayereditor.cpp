#include "backgroundlayereditor.h"
#include "visualattributemodel.h"


REGISTER_PROPERTYEDITOR("backgroundlayereditor",BackgroundLayerEditor)

BackgroundLayerEditor::BackgroundLayerEditor(QObject *parent) :
    VisualAttributeEditor("backgroundlayereditor",TR("Background Layer"),QUrl("BackgroundLayerEditor.qml"), parent)
{

}

bool BackgroundLayerEditor::canUse(const IIlwisObject& obj, const QString& name ) const
{
    return name == "backgroundlayereditor";
}

VisualAttributeEditor *BackgroundLayerEditor::create()
{
    return new BackgroundLayerEditor();
}

