#ifndef ITEMDOMAIN_H
#define ITEMDOMAIN_H

#include "range.h"
#include "itemrange.h"

namespace Ilwis {

class ItemRange;

/*!
ItemDomains are domains that handle discrete data. For example the domain of a thematic classification. There are various
item domains that are all defined by the item type they control. This is passed as template parameter to the domain.
The range of the domain contains all the valid items for that domain.
 */
template<class C> class ItemDomain : public Domain
{
public:
    ItemDomain<C>()  {
    }
    ItemDomain(const Resource& res) : Domain(res) {
    }

    ~ItemDomain() {
    }
    Domain::Containement contains(const QVariant& val) const{
        if(item(val.toString()) != 0)
            return Domain::cSELF;

        if (!isStrict() && parent().isValid())
            if (parent()->contains(val) == Domain::cSELF)
                return Domain::cPARENT;
        return Domain::cNONE;
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
        return _range->value(v.toInt());
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
        return _range->item(nam) ;
    }
    /*!
     Adds an item of the templated type to the range. If no range is yet defined, one will be created
     * \param the item to be added. Note that ownership of the item is transferred to the range. no delete allowed
     *
     */
    void addItem(DomainItem* thing) {
        if ( _range.isNull()) {
            _range.reset(C::createRange());
        }
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
        _range->remove(nme);
    }

    void setRange(const ItemRange& range)
    {
        _range.reset(C::createRange());
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


    IlwisTypes ilwisType() const {
        return itITEMDOMAIN;
    }

    IlwisTypes valueType() const {
        return C::valueTypeS();
    }

protected:
    SPRange getRange() const{
        return _range.dynamicCast<Range>();
    }

private:
    SPItemRange _range;
    QString _theme;
};

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
