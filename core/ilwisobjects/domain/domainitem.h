#ifndef DOMAINITEM_H
#define DOMAINITEM_H

namespace Ilwis {
class DomainItem
{
public:
    DomainItem() : _raw(iUNDEF) {}
    virtual ~DomainItem() {}

    virtual bool isValid() const = 0;
    virtual QString name() const =0 ;
    virtual DomainItem *clone() const=0;
    virtual IlwisTypes valueType() const = 0;
    virtual quint32 raw() const
    {
        return _raw;
    }
protected:
     quint32 _raw;

};

typedef QSharedPointer<DomainItem> SPDomainItem;
}

#endif // DOMAINITEM_H
