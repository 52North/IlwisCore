#ifndef OPENATTRIBUTETABLE_H
#define OPENATTRIBUTETABLE_H


#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/attributeeditor.h"

class OpenAttributeTable : public VisualAttributeEditor
{
    Q_OBJECT

    Q_PROPERTY(QStringList attributeTable READ attributeTable CONSTANT)

public:
    OpenAttributeTable(QObject *parent = 0);

    bool canUse(const IIlwisObject &obj, const QString &name) const;

    QStringList attributeTable() const;

    static VisualAttributeEditor *create();



  NEW_PROPERTYEDITOR(OpenAttributeTable)

private:
};

#endif // OPENATTRIBUTETABLE_H
