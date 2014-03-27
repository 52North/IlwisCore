#ifndef DOMAINITEM_H
#define DOMAINITEM_H

namespace Ilwis {
class DomainItem
{
public:
    /**
     * constructs an empty DomainItem
     */
    DomainItem(quint32 rawvalue=iUNDEF) : _raw(rawvalue) {}

    virtual ~DomainItem() {}

    /**
     * Checks the validity of this DomainItem<br>
     * when a item is valid depends on the implementation
     *
     * @return true when valid
     */
    virtual bool isValid() const = 0;

    /**
     * Query for the name of this DomainItem<br>
     * a name should never be null, but besides that it all depends on the implementation
     * @return the name
     */
    virtual QString name() const =0 ;

    /**
     * Clones this DomainItem<br>
     * a clone should fully copy all the internal members of a item onto the new item
     *
     * @return a clone of this
     */
    virtual DomainItem *clone() const=0;

    //@override
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
     *
     * @return the raw value
     */
    virtual quint32 raw() const
    {
        return _raw;
    }

    template<typename ItemType> ItemType* toType(){
        return dynamic_cast<ItemType *>(this);
    }

protected:
     quint32 _raw;

};

typedef QSharedPointer<DomainItem> SPDomainItem;
}

#endif // DOMAINITEM_H
