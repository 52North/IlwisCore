#include "coverage.h"
#include "layeropacitysetter.h"
#include "visualattributemodel.h"


REGISTER_PROPERTYEDITOR("layeropacitysetter",LayerOpacitySetter)

LayerOpacitySetter::LayerOpacitySetter(QObject *parent) :
    VisualAttributeEditor("layeropacitysetter",TR("Opacity"),QUrl("LayerOpacity.qml"), parent)
{

}

bool LayerOpacitySetter::canUse(const IIlwisObject& obj, const QString& name ) const
{
    if (!obj.isValid())
        return false;
    if(!hasType(obj->ilwisType(), itCOVERAGE))
        return false;
    return name == VisualAttributeModel::LAYER_ONLY;

}

VisualAttributeEditor *LayerOpacitySetter::create()
{
    return new LayerOpacitySetter();
}

float LayerOpacitySetter::opacity() const
{
    if ( attribute()->layer())
        return attribute()->layer()->drawer()->attribute("opacity").toFloat();
    return 1.0;
}

void LayerOpacitySetter::opacity(float tr)
{
    if (!attribute()->layer())
        return;

    if ( tr >= 0 && tr <= 1.0){
        attribute()->layer()->drawer()->setAttribute("opacity", tr);
        attribute()->layer()->drawer()->redraw();
    }
}

