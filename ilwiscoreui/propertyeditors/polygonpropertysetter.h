#ifndef POLYGONPROPERTYSETTER_H
#define POLYGONPROPERTYSETTER_H

#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/propertyeditor.h"

class PolygonPropertySetter : public PropertyEditor
{
public:
    ~PolygonPropertySetter();
    PolygonPropertySetter(QObject *parent=0);
    bool canUse(const IIlwisObject &obj) const;
    static PropertyEditor *create();

  NEW_PROPERTYEDITOR(PolygonPropertySetter)
};

#endif // POLYGONPROPERTYSETTER_H
