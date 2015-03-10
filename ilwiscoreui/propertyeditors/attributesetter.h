#ifndef ATTRIBUTESETTER_H
#define ATTRIBUTESETTER_H

#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/propertyeditor.h"

class AttributeSetter : public PropertyEditor
{
    Q_OBJECT
    Q_PROPERTY(QStringList attributes READ attributes NOTIFY attributesChanged)

public:
    AttributeSetter(QObject *parent=0);
    ~AttributeSetter();

    bool canUse(const Ilwis::IIlwisObject& obj) const;
    QStringList attributes();
    void prepare(const IIlwisObject &obj);

    NEW_PROPERTYEDITOR(AttributeSetter)

    static PropertyEditor *create();
signals:
    void attributesChanged();

private:
     QList<PropertyEditor *> _propertyEditors;
     QStringList _attributes;
};

#endif // ATTRIBUTESETTER_H
