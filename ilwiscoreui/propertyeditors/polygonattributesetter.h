#ifndef POLYGONPROPERTYSETTER_H
#define POLYGONPROPERTYSETTER_H

#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/attributeeditor.h"

class PolygonPropertySetter : public VisualAttributeEditor
{
public:
    ~PolygonPropertySetter();
    PolygonPropertySetter(QObject *parent=0);
    bool canUse(const IIlwisObject &obj) const;
    static VisualAttributeEditor *create();

  NEW_PROPERTYEDITOR(PolygonPropertySetter)
};

#endif // POLYGONPROPERTYSETTER_H
