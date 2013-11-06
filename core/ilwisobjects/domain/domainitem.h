#ifndef DOMAINITEM_H
#define DOMAINITEM_H

namespace Ilwis {
class DomainItem
{
public:
    /**
     * constructs an empty DomainItem
     */
    DomainItem() : _raw(iUNDEF) {}

    virtual ~DomainItem() {}

    /**
     * Checks if this DomainItemis valid
     * @return true when valid
     */
    virtual bool isValid() const = 0;

    /**
     * Query for the name of this DomainItem
     * @return the name
     */
    virtual QString name() const =0 ;

    /**
     * Clones this DomainItem
     * @return a clone of this
     */
    virtual DomainItem *clone() const=0;

    /**
     * Returns the IlwisTypes value of this DomainItem
     * @sa IlwisTypes
     * @return the type of this DomainItem
     */
    virtual IlwisTypes valueType() const = 0;

    /**
     * sets the raw value for this DomainItem
     *
     * @param r the new raw value
     */
    virtual void raw(quint32 r) {
        _raw = r;
    }

    /**
     * Query for the raw value of this DomainItem
     * @return the raw value
     */
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
