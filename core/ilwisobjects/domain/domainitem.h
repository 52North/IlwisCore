#ifndef DOMAINITEM_H
#define DOMAINITEM_H

namespace Ilwis {
class DomainItem
{
public:
    DomainItem() {}
    virtual ~DomainItem() {}

    virtual bool isValid() const = 0;
    //virtual QString itemType() const = 0;
    virtual QString name(quint32 index= 0) const =0 ;
    virtual DomainItem *clone() const=0;


};

typedef QSharedPointer<DomainItem> SPDomainItem;
}

#endif // DOMAINITEM_H
