#ifndef CONVERTATTRIBUTEDOMAIN_H
#define CONVERTATTRIBUTEDOMAIN_H

#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/attributeeditor.h"

class ILWISCOREUISHARED_EXPORT ConvertAttributeDomain : public VisualAttributeEditor
{
        Q_OBJECT

    Q_PROPERTY(QStringList colorSchemes READ colorSchemes CONSTANT)

public:
    ConvertAttributeDomain(QObject *parent=0);
    bool canUse(const IIlwisObject &obj, const ColumnDefinition &) const;
    static VisualAttributeEditor *create();

    Q_INVOKABLE bool execute(const QString& targetDomainType, const QString& domainName, const QString& colorScheme, const QString& viewerId);

private:
    QStringList colorSchemes() const;

 NEW_PROPERTYEDITOR(ConvertAttributeDomain)
};

#endif // CONVERTATTRIBUTEDOMAIN_H
