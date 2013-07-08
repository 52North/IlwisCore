#ifndef IDENTIFIERITEM_H
#define IDENTIFIERITEM_H

#include "Kernel_global.h"

namespace Ilwis{

class NamedIdentifierRange;

class KERNELSHARED_EXPORT IndexedIdentifier : public DomainItem
{
public:
    friend class IndexedIdentifierRange;

    IndexedIdentifier();
    IndexedIdentifier(const QString& label, quint32 ind=0, qint32 cnt =iUNDEF);
    QString name(quint32 ind = 0) const;
    quint32 index() const;
    void setIndex(quint32 ind);
    QString prefix() const;
    void setPrefix(const QString& pf);
    bool operator==(const IndexedIdentifier& item) const;
    DomainItem *clone() const;

//    QString itemType() const;
    bool isValid() const;
    static ItemRange *createRange();
    static IlwisTypes valueType();
private:
    quint32 _index;
    QString _prefix;
    quint32 _count;

};

class KERNELSHARED_EXPORT NamedIdentifier : public DomainItem {
public:
    NamedIdentifier();
    NamedIdentifier(const QString& name);
    bool isValid() const;
    QString name(quint32 n = 0) const;
    void setName(const QString& n);
    bool operator==(const NamedIdentifier& item) const;
//    QString itemType() const;
    DomainItem *clone() const;
    static Ilwis::NamedIdentifierRange *createRange();
    static IlwisTypes valueType();

private:
    QString _name;
};

typedef QSharedPointer<NamedIdentifier> SPNamedIdentifier;
typedef QSharedPointer<IndexedIdentifier> SPIndexedIdentifier;
}

#endif // IDENTIFIERITEM_H
