#ifndef IDENTIFIERRANGE_H
#define IDENTIFIERRANGE_H

#include "Kernel_global.h"
#include <QVector>

namespace Ilwis {

class NamedIdentifier;

class KERNELSHARED_EXPORT IndexedIdentifierRange : public ItemRange
{
public:
    IndexedIdentifierRange();

    bool contains(const QString& name) const;
    bool isValid() const;
    bool operator==(const IndexedIdentifierRange& range);
    void add(DomainItem *item);
    void remove(const QString& nm);
    DomainItem *item(qint64 index) const;
    DomainItem *item(const QString &nam) const;
    Range *clone() const;

    QString value(quint32 index) const;
    quint32 count() const;


private:
   IndexedIdentifier _start;
   quint32 _count;
};

class KERNELSHARED_EXPORT NamedIdentifierRange : public ItemRange
{
public:
    NamedIdentifierRange();
    ~NamedIdentifierRange();

    bool contains(const QString& name) const;
    bool isValid() const;
    void add(DomainItem *item);
    void remove(const QString& item);
    NamedIdentifierRange& operator<<(const QString& itemdef);
    bool operator==(const ItemRange& range) const;

    QString value(quint32 index) const;
    DomainItem *item(quint32 index) const;
    DomainItem *item(const QString &nam) const;
    quint32 count() const;
    Range *clone() const;

    QString toString() const;

private:
    QHash<QString, NamedIdentifier *> _items;
    QVector<NamedIdentifier *> _indexes;
};
}

#endif // IDENTIFIERRANGE_H
