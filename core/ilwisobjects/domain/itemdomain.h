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
    ItemDomain<C>() : _range(0){
    }
    ItemDomain(const Resource& res) : Domain(res), _range(0) {
    }

    ~ItemDomain() {
        delete _range;
    }
    Domain::Containement contains(const QString& val) const{
        if(item(val) != 0)
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
    QString value(double index) const {
        if (!_range) {
            ERROR1(ERR_NO_INITIALIZED_1, name());
            return sUNDEF;
        }
       return _range->value(index);
    }
    /*!
     returns a pointer to the domain item pointed to by the index
     * \param 0 based index, if the index is invalid 0 will be returned
     * \return a (pointer to) domain item or 0 if no items are defined
     */
    C* item(quint32 index) const {
        if (!_range) {
            ERROR1(ERR_NO_INITIALIZED_1, name());
            return 0;
        }
        return static_cast<C *>(_range->item(index)) ;
    }
    /*!
     returns a pointer to the item that is identified by the string name
     * \param a name that must be in the range of the domain. If not a 0 pointer will be returned
     * \return a (pointer to) domain item or 0 if no items are defined
     */
    C* item(const QString& nam) const{
        if (!_range) {
            ERROR1(ERR_NO_INITIALIZED_1, name());
            return 0;
        }
        return static_cast<C *>(_range->item(nam)) ;
    }
    /*!
     Adds an item of the templated type to the range. If no range is yet defined, one will be created
     * \param the item to be added. Note that ownership of the item is transferred to the range. no delete allowed
     *
     */
    void addItem(C* thing) {
        if ( !_range) {
            _range = C::createRange();
        }
       _range->add(thing);
    }
    /*!
     removes an item from the range
     * \param the item to be removed
     */
    void removeItem(const QString& nme){
        if (!_range) {
            ERROR1(ERR_NO_INITIALIZED_1, name());
            return ;
        }
        _range->remove(nme);
    }

    void addRange(const ItemRange& range)
    {
        for(quint32 i=0; i < range.count(); ++i) {
            addItem(range.item(i));
        }
    }
    quint32 count() const {
        if (!_range) {
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
        return ilwisType();
    }

protected:
    Range *getRange(){
        return _range;
    }

private:
    ItemRange *_range;
    QString _theme;
};

}

#endif // ITEMDOMAIN_H
