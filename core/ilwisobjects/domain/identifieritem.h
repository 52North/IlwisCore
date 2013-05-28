#ifndef IDENTIFIERITEM_H
#define IDENTIFIERITEM_H

#include "Kernel_global.h"

namespace Ilwis{

class NamedIdentifierRange;

class KERNELSHARED_EXPORT IndexedIdentifier : public DomainItem
{
public:
    IndexedIdentifier();
    IndexedIdentifier(const QString& label, quint32 ind=0);
    QString name(quint32 ind = 0) const;
    quint32 index() const;
    void setIndex(quint32 ind);
    QString prefix() const;
    void setPrefix(const QString& pf);
    bool operator==(const IndexedIdentifier& item) const;
    DomainItem *clone() const;

    QString itemType() const;
    bool isValid() const;
private:
    quint32 _index;
    QString _prefix;

};

class KERNELSHARED_EXPORT NamedIdentifier : public DomainItem {
public:
    NamedIdentifier();
    NamedIdentifier(const QString& name);
    bool isValid() const;
    QString name(quint32 n = 0) const;
    void setName(const QString& n);
    bool operator==(const NamedIdentifier& item) const;
    QString itemType() const;
    static Ilwis::NamedIdentifierRange *createRange();
    DomainItem *clone() const;

private:
    QString _name;
};
}

#endif // IDENTIFIERITEM_H
