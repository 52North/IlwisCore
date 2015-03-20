#include "visualattributemodel.h"
#include "mapinformationattributesetter.h"

REGISTER_PROPERTYEDITOR("mapinfopropertyeditor",MapInformationPropertySetter)

MapInformationPropertySetter::MapInformationPropertySetter(QObject *parent) :
    VisualAttributeEditor("mapinfopropertyeditor",TR("Mouse over Info"),QUrl("MapinfoProperties.qml"), parent)
{

}

MapInformationPropertySetter::~MapInformationPropertySetter()
{

}

bool MapInformationPropertySetter::canUse(const Ilwis::IIlwisObject &obj, const Ilwis::ColumnDefinition& ) const
{
    if ( !obj.isValid())
        return false;

    return hasType(obj->ilwisType(), itCOVERAGE)    ;
}

bool MapInformationPropertySetter::canUse(const IIlwisObject& obj, const QString& name ) const
{
    if (!obj.isValid())
        return false;
    if(!hasType(obj->ilwisType(), itCOVERAGE))
        return false;
    return name == VisualAttributeModel::LAYER_ONLY;

}

VisualAttributeEditor *MapInformationPropertySetter::create()
{
    return new MapInformationPropertySetter();
}

void MapInformationPropertySetter::prepare(CoverageLayerModel *parentLayer, const IIlwisObject &bj, const ColumnDefinition &datadef)
{

}

