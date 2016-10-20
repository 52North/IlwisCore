#ifndef ITEMDOMAIN_H
#define ITEMDOMAIN_H

#include "range.h"
#include "itemrange.h"
#include "itemiterator.h"
#include "domain.h"
#include "domainitem.h"

namespace Ilwis {

class ItemRange;

/*!
 * An item domain is a domain that contains discrete items (duh).  The items determine the value type of the domain. Item domains with different value types are not compatible. We have the following value (and thus Item) types
 *
 * - %NamedIdentifier
 * - %IndexedIdentifier
 * - Thematic
 * - Numeric
 * - %Time
 *
 * The differences between the first three are rather subtle. They are all just labels. First of all, identifiers have no attached meaning. They are labels, they identify something. As such, a  street sign has no attached meaning apart from the fact that it marks a location.
 * From the name itself you don’t get extra information. The difference between named and indexed easier. Named are unique names (in the domain), indexed are prefixes plus a number.
 * Thematic items are also labels but they have meaning. Their name implies the value they represent.  As such, these items may have metadata associated with them further defining the item.
 * The difference is quite subtle but it has effects. For example, because a thematic item has meaning it makes sense to have a standard (UI) presentation  for it. This makes no sense for an identifier item as the name implies nothing.
 * Numeric items are ordered  items that have numeric sub-ranges. For example 0-100,100-200,300-400. Time Interval Items are ordered sets of time intervals. For example 20090301-20090311, 20090312-20090322,20090323-20090402. They are very similar to numerical items .
 * Item domains have a property called “theme”. In a way this is a poor man’s parent domain. In cases where it is not feasible or practical to define a whole parent domain (e.g. insufficient knowledge) one can make item domains compatible by giving them the same theme. At a
 * string based representation  level of the items they become comparable but not on the programmatic level below that (see later). What is meant by this? All items in an item domain have a string representation(name).
 * If item domains have the same theme you may assume that names in the domain are pointing to things of the same type/identity.  But always remember that this is a shallow equality. The domains don’t have knowledge of each other, their internal representation is different.
 * Only parent domains enforce that also the internal representation is equal.
 * All child domains have by definition have the same theme.
 */
template<class D> class ItemDomain : public Domain
{
public:
    template<typename T> friend class ItemIterator;

    /*!
     * Creates an empty ItemDomain of type D
     */
    ItemDomain<D>()  {
    }

    /*!
     * Creates an itemdomain from a Resource
     * \param resource The resource to be used by this ItemDomain
     */
    ItemDomain(const Resource& resource) : Domain(resource) {
    }

    ItemDomain(Range *rng) : Domain() {
        range(rng);
    }

    ~ItemDomain() {
    }
    /*!
     * Checks if the value is contained in either this domain or the parentdomain.
     *
     * \sa Domain
     * \param val The value to be checked
     * \return cPARENT when the value is contained in the parent and this domain is not strict, cSELF if the value is contained in this domain and cNONE if the value is contained in neither
     */
    Domain::Containement contains(const QVariant& val) const{

        if (_range.isNull()) {
            ERROR1(ERR_NO_INITIALIZED_1, name());
            return Domain::cNONE;
        }
        if(_range->contains(val))
            return Domain::cSELF;

        if ( parent().isValid() && !isStrict()){

            IDomain dm = parent();
            IlwisData<ItemDomain<D>> parentdom = dm.as<ItemDomain<D>>();
            if ( parentdom->contains(val))
                return Domain::cPARENT;
        }


         return Domain::cNONE;
    }

    /*!
     * Checks if this itemdomain is compatible  with another domain,
     * will not be compatible if the other domain is:
     * - invalid
     * - not an itemdomain
     * - has another or an undefined theme (both the parent of the other domain and itself)
     *
     * \param dom the domain to be tested
     * \return true when compatible
     */
    bool isCompatibleWith(const IlwisObject* obj, bool strict=false) const {
        if ( !obj || !obj->isValid())
            return false;
        if(obj->ilwisType() != itITEMDOMAIN)
            return false;
        if ( id() == obj->id())
            return true;
        IDomain dom;
        dom.prepare(obj->id());
        if (!dom.isValid())
            return false;

        IlwisTypes tp2 = dom->valueType();
        IlwisTypes tp1 = valueType();
        if ( tp1 != tp2)
            return false;
        IlwisData<ItemDomain<D>> itemdom = dom.as<ItemDomain<D>>();
        if ( itemdom->theme() ==sUNDEF && !parent().isValid()){
            IlwisData<ItemDomain<D>> me;
            me.prepare(id());
            ItemIterator<D> iter(me);
            while(iter != iter.end()){
                ItemIterator<D> iter2(itemdom);
                bool ok = false;
                while(iter2 != iter2.end()){
                    D *v1 = *iter;
                    D *v2 = *iter2;
                    if ( v1->name() == v2->name())        {
                        ok = true;
                        break;
                    }
                    ++iter2;
                }
                if(!ok){
                    return false;
                }
                ++iter;
            }
            return true;
        }
        if ( parent().isValid() ){
            if (parent()->isCompatibleWith(obj) )
                return true;
        }
        if ( itemdom->parent().isValid())        {
            if ( itemdom->parent()->isCompatibleWith(this))
                return true;
        }
        return itemdom->theme() == theme();

    }

    /*!
     * returns a string representation of the item pointed to by the index
     *
     * \param 0 based index, if the index is invalid sUNDEF will be returned
     * \return the string representation or sUNDEDF in case no items are defined
     */
    QVariant impliedValue(const QVariant& v) const {
        if (_range.isNull()) {
            ERROR1(ERR_NO_INITIALIZED_1, name());
            return sUNDEF;
        }
        return _range->impliedValue(v);
    }

    /*!
     * returns a pointer to the domain item pointed to by the index
     *
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
     * returns a pointer to the item that is identified by the string name
     *
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
            IlwisData<ItemDomain<D>> parentdom = dm.as<ItemDomain<D>>();
            return parentdom->item(nam);
        }
        return _range->item(nam) ;
    }

    /*!
     * Adds an item of the templated type to the range. If no range is yet defined, one will be created
     *
     * \param the item to be added. Note that ownership of the item is transferred to the range. no delete allowed
     *
     */
    void addItem(DomainItem* thing) {
        if (thing == 0)
            return;
        if ( isReadOnly())
            return;
        changed(true);
        if ( _range.isNull()) {
            _range.reset(D::createRange());
        }
        if (parent().isValid()) {
            IDomain dm = parent();
            IlwisData<ItemDomain<D>> parentdom = dm.as<ItemDomain<D>>();
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
     * removes an item from the range
     *
     * \param the item to be removed
     */
    void removeItem(const QString& nme){
        if ( isReadOnly())
            return;
        if (_range.isNull()) {
            ERROR1(ERR_NO_INITIALIZED_1, name());
            return ;
        }
        changed(true);
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

    /**
     * Tries to set a new range on this ItemDomain
     * Will fail if the range doesnt fit
     *
     * @param rng the new range
     */
    void range(Range *rng){
        if ( isReadOnly())
            return;
        changed(true);
        if ( rng && rng->valueType() == valueType()){
            _range.reset(static_cast<ItemRange *>(rng));
        }
    }

    /*!
     * Sets a new range on this ItemDomain, if the excisting range / parent range / child range contain the new range
     *
     * \param range The new ItemRange
     */
    void setRange(const ItemRange& range)
    {
        if ( isReadOnly())
            return;
        changed(true);
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
            IlwisData<ItemDomain<D>> itemdom = dm.as<ItemDomain<D>>();
            if (!itemdom->_range->contains(range)){
                WARN2(WARN_NOT_PART_OF2,"item range", TR("parent domain"));
                return  ;
            }
        }

        _range.reset(D::createRange());
        _range->addRange(range);
    }

    /*!
     * Query for the amount of items in the ItemRange of this ItemDomain
     *
     * \return the amount of items in the ItemRange of this domain or undefined if there are no items in the range
     */
    quint32 count() const {
        if (_range.isNull()) {
            ERROR1(ERR_NO_INITIALIZED_1, name());
            return iUNDEF;
        }
        return _range->count();
    }

    /*!
     * Query for the theme of this ItemDomain
     * \return The theme of this ItemDomain
     */
    QString theme() const {
        return _theme;
    }

    /*!
     * Changes the Theme of this ItemDomain
     * \param theme The new Theme of this Itemdomain
     */
    void setTheme(const QString& theme) {
        if ( isReadOnly())
            return;
        changed(true);
        _theme = theme;
    }

    /*!
     * Changes the parent IDomain of this ItemDomain.
     * If the new IDomain is invalid and this ItemDomain has a parent, the parent will be removed and a raw ItemDomain remains
     * If the new domain does not meet the requirements but is valid nothing will happen
     *
     * \param dm The new Domain
     */
    void setParent(const IDomain& dm){
        if ( isReadOnly())
            return;
        changed(true);
        if ( !dm.isValid() && parent().isValid()){
            // cut the relation with the parent; raws remain the same but no relation with parent any more;
            IDomain dmp = parent();
            IlwisData<ItemDomain<D>> dmparent = dmp.as<ItemDomain<D>>();
            dmparent->removeChildDomain(id());
            _range.reset(static_cast<ItemRange *>(_range->clone()));
            Domain::setParent(dm);
            return ;
        }
        if ( _range.isNull()) {
            _range.reset(D::createRange());
        }
        if ( !hasType(dm->ilwisType(), itITEMDOMAIN) ) {
            return;
        }
        if (!hasType(dm->valueType(), valueType())) {
            return;
        }
        IlwisData<ItemDomain<D>> dmitem = dm.as<ItemDomain<D>>();
        if ( theme() != dmitem->theme())
            return;

        bool ok = _range->alignWithParent(dm);
        if (!ok)
            return ;

        Domain::setParent(dm);

    }

    //@override
    IlwisTypes ilwisType() const {
        return itITEMDOMAIN;
    }

    //@override
    IlwisTypes valueType() const {
        return D::valueTypeS();
    }

    /*!
     * Create an ItemIterator over this domain
     * This ItemIterators current position is at the start
     * \return ItemIterator<D>(*this)
     */
    ItemIterator<D> begin() const {
        return ItemIterator<D>(*this);
    }

    /*!
     * Create an ItemIterator over this domain
     * This ItemIterators current position is at the end
     * \return ItemIterator<D>(*this, count())
     */
    ItemIterator<D> end() const {
        return ItemIterator<D>(*this, count());
    }

    /*!
     * Check if this is a valid domain
     * A ItemDomain is valid when the range is not null
     * \return true if this domain is valid
     */
    bool isValid() const {
        return !_range.isNull();
    }

    bool isOrdered() const{
        return false;
    }

    IlwisObject *clone(){
        auto *itemdom = new ItemDomain<D>();
        copyTo(itemdom);
        return itemdom;
    }

    void copyTo(IlwisObject *obj){
        Domain::copyTo(obj);
        ItemDomain<D> *itemdom = static_cast<ItemDomain<D> *>(obj);
        itemdom->_range.reset( static_cast<ItemRange *>(_range->clone()));
        itemdom->_theme = _theme;
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
class Interval;

typedef IlwisData<ItemDomain<ThematicItem>>  IThematicDomain ;
typedef IlwisData<ItemDomain<IndexedIdentifier>>  IIndexedIdDomain ;
typedef IlwisData<ItemDomain<NamedIdentifier>>  INamedIdDomain ;
typedef IlwisData<ItemDomain<Interval>>  IIntervalDomain ;
typedef ItemDomain<ThematicItem>  ThematicDomain ;
typedef ItemDomain<IndexedIdentifier>  IndexedIdDomain ;
typedef ItemDomain<NamedIdentifier>  NamedIdDomain ;
typedef ItemDomain<Interval>  IntervalDomain ;
typedef IlwisData<ItemDomain<DomainItem>> IItemDomain;

}



#endif // ITEMDOMAIN_H
