#include "kernel.h"
#include "ilwisdata.h"
#include "representation.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "visualattributemodel.h"
#include "coveragelayermodel.h"
#include "globallayermodel.h"
#include "uicontextmodel.h"

const QString VisualAttributeModel::LAYER_ONLY = "ilwis_layer_only_attribute";
const QString VisualAttributeModel::GLOBAL_ONLY = "ilwis_global_layer_only_attribute";

VisualAttributeModel::VisualAttributeModel()
{

}

VisualAttributeModel::~VisualAttributeModel()
{

}

VisualAttributeModel::VisualAttributeModel(const Ilwis::ColumnDefinition &def, CoverageLayerModel *parentLayer, const Ilwis::IIlwisObject &obj) :
    AttributeModel( def,parentLayer,obj)
{
    _layer = parentLayer;
    if ( def.isValid()){
        QVariant actAttribute = attributename();// layer()->drawer()->attribute("activevisualattribute");
        if ( !actAttribute.isValid())
            return ;
        QVariant var = layer()->drawer()->attribute("visualattribute|representation|" + actAttribute.toString());
        _representation = var.value<IRepresentation>();
        _propertyEditors = uicontext()->propertyEditors(this, obj, def);
    }
}

IRepresentation VisualAttributeModel::representation() const
{
    return _representation;

}

void VisualAttributeModel::representation(const IRepresentation &rpr)
{
    _representation = rpr;
    for(auto *editor : _propertyEditors){
        editor->representationChanged(rpr);
    }
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

CoverageLayerModel *VisualAttributeModel::layer() const
{
    return _layer;
}

CoverageLayerModel *VisualAttributeModel::layer()
{
    return _layer;
}


//--------------------------------------------------------------------------
LayerAttributeModel::LayerAttributeModel(CoverageLayerModel *parentLayer, const IIlwisObject &obj) :
    VisualAttributeModel(Ilwis::ColumnDefinition(), parentLayer, obj)
{
   _propertyEditors = uicontext()->propertyEditors(this, obj, VisualAttributeModel::LAYER_ONLY);
}

QString LayerAttributeModel::attributename() const
{
    return TR("Layer Wide");
}

//--------------------------------------------
GlobalAttributeModel::GlobalAttributeModel(const QString &label, const QString& associatedEditor, GlobalLayerModel *parentLayer) :
    VisualAttributeModel(Ilwis::ColumnDefinition(), parentLayer, IIlwisObject()),
    _label(label)
{
        _propertyEditors.append(uicontext()->propertyEditors(this, IIlwisObject(), associatedEditor));
}

QString GlobalAttributeModel::attributename() const
{
    return _label;
}


