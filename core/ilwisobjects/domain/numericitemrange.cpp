#include <QString>
#include <typeinfo>
#include "kernel.h"
#include "range.h"
#include "domainitem.h"
#include "numericrange.h"
#include "domainitem.h"
#include "numericitem.h"
#include "itemrange.h"
#include "numericitemrange.h"

using namespace Ilwis;

NumericItemRange::NumericItemRange()
{
    _vt = itNUMERICITEM;
}

QString NumericItemRange::value(quint32 index) const
{
    if ( index < _items.size())
        return _items[index]->name();
    return sUNDEF;

}

quint32 NumericItemRange::count() const
{
    return _items.size();
}

SPDomainItem NumericItemRange::item(quint32 index) const
{
       if ( index < _items.size())
           return _items[index];
       return SPDomainItem();
}

SPDomainItem NumericItemRange::item(const QString &item) const
{
    QStringList parts = item.split(" ");
    if ( parts.size() == 0 || parts.size() > 3){
        ERROR2(ERR_ILLEGAL_VALUE_2, TR("numeric range"), item);
        return SPDomainItem();
    }
    bool ok;
    double v1 = index(parts[0].toDouble(&ok));
    if ( v1 == rUNDEF  && ok) {
        ERROR2(ERR_ILLEGAL_VALUE_2, TR("numeric range"), item);
        return SPDomainItem();
    }
    if (parts.size() > 1) {
        double v2 = index(parts[1].toDouble(&ok));
        if ( !ok || ((int)v1 - (int)v2 != 0)) {
            ERROR2(ERR_ILLEGAL_VALUE_2, TR("numeric range"), item);
            return SPDomainItem();
        }
    }
    return _items[(int)v1];
}

SPDomainItem NumericItemRange::itemByOrder(quint32 index) const
{
    return item(index);
}

double NumericItemRange::index(double v) const
{
    if (!isValid())
        return rUNDEF;

    for(quint32 ind = 0; ind < _items.size(); ++ind) {
        quint32 mark1=iUNDEF, mark2=iUNDEF;
        const NumericRange& rng = _items[ind]->range();
        if ( v >= rng.min()){
            mark1 = ind;
        }
        if ( v <= rng.max()){
            mark2 = ind;
        }
        if ( mark1 != iUNDEF && mark2 != iUNDEF) {
            bool ok = isContinuous();
            if (ok ) {
                return ind + (v - rng.min()) / rng.distance();
            }
            return ind;
        }
    }
    return rUNDEF;
}

bool NumericItemRange::validNumber(QString value) const{
    bool oknumber;
    double v = value.toDouble(&oknumber);
    if (!oknumber) {
        return ERROR2(ERR_ILLEGAL_VALUE_2, TR("numeric range"), value);
    }

    return index(v) != rUNDEF;

}

bool NumericItemRange::contains(const QString &name, bool ) const
{
    QStringList items = name.split(";");
    for(const QString& item : items) {
        QStringList parts = item.split(" ");
        if ( parts.size() == 0 || parts.size() > 3){
            return ERROR2(ERR_ILLEGAL_VALUE_2, TR("numeric range"), name);
        }
        bool ok = validNumber(parts[0]);
        if (parts.size() > 1)
            ok &= validNumber(parts[1]);
        if (!ok)
            return false;
    }
    return true;
}

bool NumericItemRange::isValid() const
{
    return _items.size() != 0;
}

void NumericItemRange::add(DomainItem *item)
{
    if (!item)
        return;

    if ( item->valueType() != itNUMERICITEM)
        return;

    if (!item->isValid())
        return;
    SPNumericItem nitem(static_cast<NumericItem *>(item));
    for(auto iter = _items.rbegin(); iter != _items.rend(); ++iter) {
        if ( nitem->range() > (*iter)->range()) {
            if ( nitem->raw() == iUNDEF)
                nitem->_raw = _items.size();
            _items.insert(iter.base(),1, nitem );
            return;
        }
    }
    if ( _items.size() == 0) { // no overlapping items allowed; so the only case that is legal here is the first
        nitem->_raw = 0;
        _items.push_back(nitem);
    }

}

void NumericItemRange::remove(const QString &nm)
{
     for(auto iter = _items.begin(); iter != _items.begin(); ++iter) {
        if ( (*iter)->name() == nm) {
            _items.erase(iter);
        }
     }
}

QString NumericItemRange::toString() const
{   if ( _items.size() == 0)
        return sUNDEF;
    QString names;
    for(const SPNumericItem& it: _items) {
        if ( names != "")
            names += ";";
        names += it->name();
    }
    return names;
}

ItemRange *NumericItemRange::clone() const
{
    ItemRange *items = new NumericItemRange();
    for(const SPNumericItem& it: _items) {
        items->add(it->clone());
    }
    items->interpolation(interpolation());

    return items;

}

NumericItemRange &NumericItemRange::operator <<(const QString &itemdef)
{
    QStringList parts = itemdef.split(" ");
    if( parts.size() > 1) {
        bool ok;
        double step = 0;
        double mn = parts[0].toDouble(&ok);
        if(ok) {
            double mx = parts[1].toDouble(&ok);
            if ( ok) {
                if ( parts.size() == 3) {
                    step = parts[2].toDouble(&ok);

                }
                if (ok) {
                    add(new NumericItem({mn,mx, step}));
                    return *this;
                }
            }
        }
        ERROR1(ERR_NO_INITIALIZED_1, "numeric item range");
    } if ( parts.size() == 1) {
        if ( count() == 0) {
            ERROR1(ERR_NO_INITIALIZED_1, "numeric item range");
            return *this;
        }
        bool ok;
        double vmin = _items[_items.size() - 1]->range().max();
        double vmax = itemdef.toDouble(&ok) ;
        if (!ok) {
            ERROR1(ERR_NO_INITIALIZED_1, "numeric item range");
        }
        double step = _items[0]->range().step();
        if ( step == 0)
            vmin += EPS8;
        add(new NumericItem({vmin+step,vmax, step}));
    }
    return *this;
}

void NumericItemRange::addRange(const ItemRange &range)
{
    ItemRange::addRange(range);
    _interpolation = static_cast<const NumericItemRange&>(range).interpolation();
}

