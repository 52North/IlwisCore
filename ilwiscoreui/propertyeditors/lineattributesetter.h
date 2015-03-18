#ifndef LINEPROPERTYSETTER_H
#define LINEPROPERTYSETTER_H


#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/attributeeditor.h"

class LinePropertySetter : public VisualAttributeEditor
{
public:
    ~LinePropertySetter();
    LinePropertySetter(QObject *parent=0);
    bool canUse(const IIlwisObject &obj, const DataDefinition &) const;
    static VisualAttributeEditor *create();

 NEW_PROPERTYEDITOR(LinePropertySetter)
};

#endif // LINEPROPERTYSETTER_H
