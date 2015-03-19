#include "coverage.h"
#include "layertransparencysetter.h"
#include "visualattributemodel.h"


REGISTER_PROPERTYEDITOR("layertransparencysetter",LayerTransparencySetter)

LayerTransparencySetter::LayerTransparencySetter(QObject *parent) :
    VisualAttributeEditor("layertransparencysetter",TR("Transparency"),QUrl("LayerTransparency.qml"), parent)
{

}

LayerTransparencySetter::~LayerTransparencySetter()
{

}

bool LayerTransparencySetter::canUse(const IIlwisObject& obj, const QString& name ) const
{
    if (!obj.isValid())
        return false;
    if(!hasType(obj->ilwisType(), itCOVERAGE))
        return false;
    return name == VisualAttributeModel::LAYER_ONLY;

}

VisualAttributeEditor *LayerTransparencySetter::create()
{
    return new LayerTransparencySetter();
}

float LayerTransparencySetter::transparency() const
{
    if ( layer())
        return 1.0 - layer()->drawer()->attribute("alphachannel").toFloat();
    return 0.0;
}

void LayerTransparencySetter::transparency(float tr)
{
    if (!layer())
        return;

    if ( tr >= 0 && tr <= 1.0){
        layer()->drawer()->setAttribute("alphachannel", 1.0 - tr);
        layer()->drawer()->redraw();
    }
}

