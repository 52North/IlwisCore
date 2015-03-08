#include "mapinformationpropertysetter.h"

REGISTER_PROPERTYEDITOR("mapinfopropertyeditor",MapInformationPropertySetter)

MapInformationPropertySetter::MapInformationPropertySetter(QObject *parent) :
    PropertyEditor("mapinfopropertyeditor",TR("Map Information"),QUrl("MapinfoProperties.qml"), parent)
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

PropertyEditor *MapInformationPropertySetter::create()
{
    return new MapInformationPropertySetter();
}

