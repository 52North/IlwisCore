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
    QString name() const;
    quint32 raw() const;
    //void setIndex(quint32 ind);
    QString prefix() const;
    void setPrefix(const QString& pf);
    bool operator==(const IndexedIdentifier& item) const;
    DomainItem *clone() const;
    IlwisTypes valueType() const;

    bool isValid() const;
    static ItemRange *createRange();
    static IlwisTypes valueTypeS();

private:
    quint32 _raw;
    QString _prefix;
    quint32 _count;

};

class KERNELSHARED_EXPORT NamedIdentifier : public DomainItem {
public:
    friend class NamedIdentifierRange;

    NamedIdentifier();
    NamedIdentifier(const QString& name);
    bool isValid() const;
    QString name() const;
    void setName(const QString& n);
    bool operator==(const NamedIdentifier& item) const;
    IlwisTypes valueType() const;
    quint32 raw() const;

    DomainItem *clone() const;
    static Ilwis::NamedIdentifierRange *createRange();
    static IlwisTypes valueTypeS();

private:
    QString _name;
    quint32 _raw;
};

typedef QSharedPointer<NamedIdentifier> SPNamedIdentifier;
typedef QSharedPointer<IndexedIdentifier> SPIndexedIdentifier;
}

#endif // IDENTIFIERITEM_H
