#ifndef ITEMDOMAIN_H
#define ITEMDOMAIN_H

#include "range.h"
#include "itemrange.h"

namespace Ilwis {

class ItemRange;

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

    QString value(double index) const {
        if (!_range) {
            ERROR1(ERR_NO_INITIALIZED_1, name());
            return sUNDEF;
        }
       return _range->value(index);
    }
    C* item(quint32 index) const {
        if (!_range) {
            ERROR1(ERR_NO_INITIALIZED_1, name());
            return 0;
        }
        return static_cast<C *>(_range->item(index)) ;
    }

    C* item(const QString& nam) const{
        if (!_range) {
            ERROR1(ERR_NO_INITIALIZED_1, name());
            return 0;
        }
        return _range->item(nam) ;
    }
    void addItem(C* thing) {
        if ( !_range) {
            _range = C::createRange();
        }
       _range->add(thing);
    }

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

    const ItemRange *range() const{
        return _range;
    }

    IlwisTypes ilwisType() const {
        return itITEMDOMAIN;
    }

    IlwisTypes valueType() const {
        return ilwisType();
    }


private:
    ItemRange *_range;
    QString _theme;
};
}

#endif // ITEMDOMAIN_H
