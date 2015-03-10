#ifndef MAPINFORMATIONPROPERTYSETTER_H
#define MAPINFORMATIONPROPERTYSETTER_H
#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/propertyeditor.h"

class MapInformationPropertySetter : public PropertyEditor
{
public:
    MapInformationPropertySetter(QObject *parent = 0);
    ~MapInformationPropertySetter();

    bool canUse(const Ilwis::IIlwisObject& obj) const;
    static PropertyEditor *create();

    NEW_PROPERTYEDITOR(MapInformationPropertySetter)
};

#endif // MAPINFORMATIONPROPERTYSETTER_H
