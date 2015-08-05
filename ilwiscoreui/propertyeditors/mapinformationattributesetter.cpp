#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
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

bool MapInformationPropertySetter::showInfo() const
{
    if ( attribute()->layer())
        return attribute()->layer()->showInfo();
    return true;
}

void MapInformationPropertySetter::setShowInfo(bool yesno)
{
    if (!attribute()->layer())
        return;

    attribute()->layer()->showInfo(yesno);

}

