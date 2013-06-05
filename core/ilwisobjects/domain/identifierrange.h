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
    void remove(const QString& nm);
    Range *clone() const;
    SPDomainItem item(quint32 index) const;
    SPDomainItem item(const QString &nam) const;
    void add(DomainItem *item);
    QString toString() const;

    QString value(quint32 index) const;
    quint32 count() const;
    void count(quint32 nr);


private:
   SPIndexedIdentifier _start;
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
    SPDomainItem item(quint32 index) const;
    SPDomainItem item(const QString &nam) const;
    quint32 count() const;
    Range *clone() const;

    QString toString() const;

private:
    QHash<QString, SPNamedIdentifier> _items;
    QVector<SPNamedIdentifier> _indexes;
};
}

#endif // IDENTIFIERRANGE_H
