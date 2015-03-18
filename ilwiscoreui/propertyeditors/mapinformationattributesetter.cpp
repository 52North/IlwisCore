#include "visualattributemodel.h"
#include "mapinformationattributesetter.h"

REGISTER_PROPERTYEDITOR("mapinfopropertyeditor",MapInformationPropertySetter)

MapInformationPropertySetter::MapInformationPropertySetter(QObject *parent) :
    VisualAttributeEditor("mapinfopropertyeditor",TR("Map Information"),QUrl("MapinfoProperties.qml"), parent)
{

}

MapInformationPropertySetter::~MapInformationPropertySetter()
{

}

bool MapInformationPropertySetter::canUse(const Ilwis::IIlwisObject &obj, const Ilwis::DataDefinition& ) const
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

