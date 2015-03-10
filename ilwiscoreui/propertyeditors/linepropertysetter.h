#ifndef LINEPROPERTYSETTER_H
#define LINEPROPERTYSETTER_H


#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/propertyeditor.h"

class LinePropertySetter : public PropertyEditor
{
public:
    ~LinePropertySetter();
    LinePropertySetter(QObject *parent=0);
    bool canUse(const IIlwisObject &obj) const;
    static PropertyEditor *create();

 NEW_PROPERTYEDITOR(LinePropertySetter)
};

#endif // LINEPROPERTYSETTER_H
