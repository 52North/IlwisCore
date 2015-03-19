#include "visualattributemodel.h"
#include "coveragelayermodel.h"
#include "uicontextmodel.h"

const QString VisualAttributeModel::LAYER_ONLY = "ilwis_layer_only_attribute";

VisualAttributeModel::VisualAttributeModel()
{

}

VisualAttributeModel::~VisualAttributeModel()
{

}

VisualAttributeModel::VisualAttributeModel(const Ilwis::ColumnDefinition &def, CoverageLayerModel *parentLayer, const Ilwis::IIlwisObject &obj) :
    AttributeModel( def,parentLayer,obj)
{
    if ( def.isValid())
        _propertyEditors = uicontext()->propertyEditors(parentLayer, obj, def);
}

QQmlListProperty<VisualAttributeEditor> VisualAttributeModel::propertyEditors()
{
    return QQmlListProperty<VisualAttributeEditor>(this, _propertyEditors);
}

VisualAttributeEditor *VisualAttributeModel::propertyEditor(const QString &name)
{
    for(auto editor : _propertyEditors){
        if ( editor->editorName() == name)
            return editor;
    }
    return 0;
}


//--------------------------------------------------------------------------
LayerAttributeModel::LayerAttributeModel(CoverageLayerModel *parentLayer, const IIlwisObject &obj) :
    VisualAttributeModel(Ilwis::ColumnDefinition(), parentLayer, obj)
{
   _propertyEditors = uicontext()->propertyEditors(parentLayer, obj, VisualAttributeModel::LAYER_ONLY);
}

QString LayerAttributeModel::attributename() const
{
    return TR("Layer Wide");
}
