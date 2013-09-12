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

    bool contains(const QString& name, bool inclusive = true) const;
    bool isValid() const;
    bool operator==(const IndexedIdentifierRange& range);
    void remove(const QString& nm);
    Range *clone() const;
    SPDomainItem item(quint32 index) const;
    SPDomainItem item(const QString &nam) const;
    SPDomainItem itemByOrder(quint32 index) const;
    void add(DomainItem *item);
    QString toString() const;

    QString value(quint32 index) const;
    quint32 count() const;
    void count(quint32 nr);
    virtual bool isContinuous() const;
    void interpolation(const QString&) {}


    quint32 raw(const QString &item) const;

private:
   SPIndexedIdentifier _start;
   quint32 _count;
};

class KERNELSHARED_EXPORT NamedIdentifierRange : public ItemRange
{
public:
    NamedIdentifierRange();
    ~NamedIdentifierRange();

    bool contains(const QString& name, bool inclusive = true) const;
    bool isValid() const;
    void add(DomainItem *item);
    void remove(const QString& item);
    NamedIdentifierRange& operator<<(const QString& itemdef);
    bool operator==(const ItemRange& range) const;

    QString value(quint32 index) const;
    SPDomainItem item(quint32 iraw) const;
    SPDomainItem item(const QString &nam) const;
    SPDomainItem itemByOrder(quint32 index) const;
    quint32 count() const;
    Range *clone() const;

    QString toString() const;
    virtual bool isContinuous() const;
    void interpolation(const QString&) {}

private:
    std::map<QString, SPNamedIdentifier> _byName;
    std::map<quint32, SPNamedIdentifier> _byRaw;
    std::vector<SPNamedIdentifier> _byOrder;
};

class KERNELSHARED_EXPORT ThematicRange : public NamedIdentifierRange {
public:
    ThematicRange();
    ~ThematicRange() {}
    ThematicRange& operator<<(const QString& itemdef);
};
}

#endif // IDENTIFIERRANGE_H
