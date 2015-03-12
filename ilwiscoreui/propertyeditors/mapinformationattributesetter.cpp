#include "mapinformationattributesetter.h"

REGISTER_PROPERTYEDITOR("mapinfopropertyeditor",MapInformationPropertySetter)

MapInformationPropertySetter::MapInformationPropertySetter(QObject *parent) :
    VisualAttributeEditor("mapinfopropertyeditor",TR("Map Information"),QUrl("MapinfoProperties.qml"), parent)
{

}

MapInformationPropertySetter::~MapInformationPropertySetter()
{

}

bool MapInformationPropertySetter::canUse(const IIlwisObject &obj) const
{
    if ( !obj.isValid())
        return false;

    return hasType(obj->ilwisType(), itCOVERAGE)    ;
}

VisualAttributeEditor *MapInformationPropertySetter::create()
{
    return new MapInformationPropertySetter();
}

