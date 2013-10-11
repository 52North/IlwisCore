#ifndef ITEMDOMAIN_H
#define ITEMDOMAIN_H

#include "range.h"
#include "itemrange.h"
#include "itemiterator.h"

namespace Ilwis {

class ItemRange;

/*!
ItemDomains are domains that handle discrete data. For example the domain of a thematic classification. There are various
item domains that are all defined by the item type they control. This is passed as template parameter to the domain.
The range of the domain contains all the valid items for that domain.
 */
template<class D> class ItemDomain : public Domain
{
public:
    template<typename T> friend class ItemIterator;

    ItemDomain<D>()  {
    }
    ItemDomain(const Resource& resource) : Domain(resource) {
    }

    ~ItemDomain() {
    }

    Domain::Containement contains(const QVariant& val) const{
        if ( parent().isValid() && !isStrict())
            if (parent()->contains(val) == Domain::cSELF)
                return Domain::cPARENT;

        if(item(val.toString()) != 0)
            return Domain::cSELF;

         return Domain::cNONE;
    }

    bool isCompatibleWith(const IDomain& dom) const {
        if ( !dom->isValid())
            return false;
        if(dom->ilwisType() != itITEMDOMAIN)
            return false;
        if (!valueType() != dom->valueType())
            return false;
        IlwisData<ItemDomain<D>> itemdom = dom.get<ItemDomain<D>>();
        if ( itemdom->theme() ==sUNDEF && !itemdom->parent().isValid())
            return false;
        if ( itemdom->parent().isValid() && parent().isValid()){
            return itemdom->parent() == parent();
        }
        return itemdom->theme() == theme();

    }
    /*!
     returns a string representation of the item pointed to by the index
     * \param 0 based index, if the index is invalid sUNDEF will be returned
     * \return the string representation or sUNDEDF in case no items are defined
     */
    QString value(const QVariant& v) const {
        if (_range.isNull()) {
            ERROR1(ERR_NO_INITIALIZED_1, name());
            return sUNDEF;
        }
        return _range->value(v);
    }
    /*!
     returns a pointer to the domain item pointed to by the index
     * \param 0 based index, if the index is invalid 0 will be returned
     * \return a (pointer to) domain item or 0 if no items are defined
     */
    SPDomainItem item(quint32 index) const {
        if (_range.isNull()) {
            ERROR1(ERR_NO_INITIALIZED_1, name());
            return SPDomainItem();
        }
        return _range->item(index) ;
    }
    /*!
     returns a pointer to the item that is identified by the string name
     * \param a name that must be in the range of the domain. If not a 0 pointer will be returned
     * \return a (pointer to) domain item or 0 if no items are defined
     */
    SPDomainItem item(const QString& nam) const{
        if (_range.isNull()) {
            ERROR1(ERR_NO_INITIALIZED_1, name());
            return SPDomainItem();
        }
        if(parent().isValid() && !isStrict()) {
            IDomain dm = parent();
            IlwisData<ItemDomain<D>> parentdom = dm.get<ItemDomain<D>>();
            return parentdom->item(nam);
        }
        return _range->item(nam) ;
    }
    /*!
     Adds an item of the templated type to the range. If no range is yet defined, one will be created
     * \param the item to be added. Note that ownership of the item is transferred to the range. no delete allowed
     *
     */
    void addItem(DomainItem* thing) {
        if (thing == 0)
            return;
        if ( _range.isNull()) {
            _range.reset(D::createRange());
        }
        if (parent().isValid()) {
            IDomain dm = parent();
            IlwisData<ItemDomain<D>> parentdom = dm.get<ItemDomain<D>>();
            if(!parentdom.isValid()){
                ERROR2(ERR_COULD_NOT_CONVERT_2,TR("domain"), TR("correct item domain"));
                delete thing;
                return;
            }
            SPDomainItem item = parentdom->item(thing->name());
            if (item.isNull()){
                WARN2(WARN_NOT_PART_OF2,thing->name(), TR("parent domain"));
                delete thing;
                return;
            }
            delete thing;
            _range->add(item);
        }else
            _range->add(thing);
    }
    /*!
     removes an item from the range
     * \param the item to be removed
     */
    void removeItem(const QString& nme){
        if (_range.isNull()) {
            ERROR1(ERR_NO_INITIALIZED_1, name());
            return ;
        }
        if ( _childDomains.size() > 0) {
            for(auto childid: _childDomains) {
                IlwisData<ItemDomain<D>> childDom = mastercatalog()->get(childid.first) ;
                if ( childDom.isValid()) {
                    if ( childDom->contains(nme) == Domain::cSELF) {
                        WARN2(WARN_PART_OF2,nme, TR("related/child domain"));
                        return;
                    }
                }
            }
        }
        _range->remove(nme);
    }

    void setRange(const ItemRange& range)
    {
        if ( _childDomains.size() > 0) {
            for(auto childid: _childDomains) {
                IlwisData<ItemDomain<D>> childDom = mastercatalog()->get(childid.first) ;
                if ( childDom.isValid()) {
                    if(!range.contains(childDom->_range)) {
                        WARN2(WARN_NOT_PART_OF2,"item range", TR("child domain"));
                        return;
                    }
                }
            }
        }

        if ( parent().isValid()){
            IDomain dm = parent();
            IlwisData<ItemDomain<D>> itemdom = dm.get<ItemDomain<D>>();
            if (!itemdom->_range->contains(range)){
                WARN2(WARN_NOT_PART_OF2,"item range", TR("parent domain"));
                return  ;
            }
        }

        _range.reset(D::createRange());
        _range->addRange(range);
    }
    quint32 count() const {
        if (_range.isNull()) {
            ERROR1(ERR_NO_INITIALIZED_1, name());
            return iUNDEF;
        }
        return _range->count();
    }

    QString theme() const {
        return _theme;
    }
    void setTheme(const QString& theme) {
        _theme = theme;
    }

    void setParent(const IDomain& dm){
        if ( !dm.isValid() && parent().isValid()){
            // cut the relation with the parent; raws remain the same but no relation with parent any more;
            IDomain dmp = parent();
            IlwisData<ItemDomain<D>> dmparent = dmp.get<ItemDomain<D>>();
            dmparent->removeChildDomain(id());
            _range.reset(static_cast<ItemRange *>(_range->clone()));
            Domain::setParent(dm);
            return ;
        }
        if ( !hasType(dm->ilwisType(), itITEMDOMAIN) ) {
            return;
        }
        if (!hasType(dm->valueType(), valueType())) {
            return;
        }
        IlwisData<ItemDomain<D>> dmitem = dm.get<ItemDomain<D>>();
        if ( theme() != dmitem->theme())
            return;

        bool ok = _range->alignWithParent(dm);
        if (!ok)
            return ;

        Domain::setParent(dm);

    }

    IlwisTypes ilwisType() const {
        return itITEMDOMAIN;
    }

    IlwisTypes valueType() const {
        return D::valueTypeS();
    }

    ItemIterator<D> begin() const {
        return ItemIterator<D>(*this);
    }

    ItemIterator<D> end() const {
        return ItemIterator<D>(*this, count());
    }
    bool isValid() const {
        return !_range.isNull();
    }

protected:
    SPRange getRange() const{
        return _range.dynamicCast<Range>();
    }

private:
    bool considerParent() const{
        return !parent().isValid();
    }

    SPItemRange _range;
    QString _theme = sUNDEF;
};

template<typename T> Ilwis::ItemIterator<T> begin(const Ilwis::IlwisData<Ilwis::ItemDomain<T>>& idom) {
    return idom->begin();
}

template<typename T> Ilwis::ItemIterator<T> end(const Ilwis::IlwisData<Ilwis::ItemDomain<T>>& idom) {
    return idom->end();
}

class ThematicItem;
class IndexedIdentifier;
class NamedIdentifier;
class NumericItem;

typedef IlwisData<ItemDomain<ThematicItem>>  IThematicDomain ;
typedef IlwisData<ItemDomain<IndexedIdentifier>>  IIndexedIdDomain ;
typedef IlwisData<ItemDomain<NamedIdentifier>>  INamedIdDomain ;
typedef IlwisData<ItemDomain<NumericItem>>  INumericItemDomain ;
typedef ItemDomain<ThematicItem>  ThematicDomain ;
typedef ItemDomain<IndexedIdentifier>  IndexedIdDomain ;
typedef ItemDomain<NamedIdentifier>  NamedIdDomain ;
typedef ItemDomain<NumericItem>  NumericItemDomain ;

}



#endif // ITEMDOMAIN_H
