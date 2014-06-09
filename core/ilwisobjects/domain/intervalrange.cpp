#include <QString>
#include <typeinfo>
#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "domainitem.h"
#include "numericrange.h"
#include "domainitem.h"
#include "domain.h"
#include "itemdomain.h"
#include "interval.h"
#include "itemrange.h"
#include "intervalrange.h"

using namespace Ilwis;

IntervalRange::IntervalRange()
{
    _vt = itNUMERICITEM;
}

QVariant IntervalRange::impliedValue(const QVariant& v) const
{
    bool ok;
    quint32 index = v.toUInt(&ok);
    if (!ok){
        ERROR2(ERR_COULD_NOT_CONVERT_2,v.toString(), "raw value");
        return sUNDEF;
    }
    if ( index < _items.size())
        return _items[index]->name();
    return sUNDEF;

}

quint32 IntervalRange::count() const
{
    return _items.size();
}

SPDomainItem IntervalRange::item(quint32 index) const
{
       if ( index < _items.size())
           return _items[index];
       return SPDomainItem();
}

SPDomainItem IntervalRange::item(const QString &item) const
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
    if (!ok){
        for(auto val : _items) {
            if ( val->name() == item){
                return val;
            }
        }
        return SPDomainItem();
    }
    return _items[(int)v1];
}

SPDomainItem IntervalRange::itemByOrder(quint32 index) const
{
    return item(index);
}

double IntervalRange::index(double v) const
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

bool IntervalRange::validNumber(QString value) const{
    bool oknumber;
    double v = value.toDouble(&oknumber);
    if (!oknumber) {
        return false;
    }

    return index(v) != rUNDEF;

}

bool IntervalRange::contains(const QVariant &name, bool ) const
{
    QStringList items = name.toString().split(";");
    for(const QString& item : items) {
        QStringList parts = item.split(" ");
        if ( parts.size() == 0 || parts.size() > 3){
            return ERROR2(ERR_ILLEGAL_VALUE_2, TR("numeric range"), name.toString());
        }
        bool ok = validNumber(parts[0]);
        if (parts.size() > 1)
            ok &= validNumber(parts[1]);
        if (!ok && parts.size() == 1){ // maybe we checked for the label
            for(const auto& item : _items)    {
                if (item->name() == parts[0]){
                    return true;
                }
            }
        }
        return ok;
    }
    return true;
}

bool IntervalRange::isValid() const
{
    return _items.size() != 0;
}

void IntervalRange::add(DomainItem *item)
{
    if (!item)
        return;

    if ( item->valueType() != itNUMERICITEM)
        return;

    if (!item->isValid())
        return;
    SPInterval nitem(static_cast<Interval *>(item));
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

void IntervalRange::add(SPDomainItem item)
{
    if ( item.isNull() || !item->isValid() || item->valueType() != itNUMERICITEM)
        return;

    SPInterval nitem = item.staticCast<Interval>();
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

bool IntervalRange::alignWithParent(const IDomain &dom)
{
    IIntervalDomain numdom = dom.as<IntervalDomain>();
    if ( !numdom.isValid())
        return false;
    ItemIterator<Interval> iterParent(numdom);
    std::map<QString, DomainItem *> parentItems;
    for(DomainItem *item : iterParent) {
        parentItems[item->name()] = item;
    }

    for(int i=0; i < _items.size(); ++i) {
        // TODO:: a bit more flexible in the future(maybe?) with overlapping ranges; absent step sizes
        auto iter = parentItems.find(_items[i]->name());
        if ( iter == parentItems.end()){
            return ERROR2(ERR_ILLEGAL_VALUE_2, TR("item in child domain"),_items[i]->name());
        }
        _items[i] = SPInterval(static_cast<Interval *>((*iter).second->clone()));
    }
    return true;

}

void IntervalRange::remove(const QString &nm)
{
     for(auto iter = _items.begin(); iter != _items.end(); ++iter) {
        if ( (*iter)->name() == nm) {
            _items.erase(iter);
            break;
        }
     }
}

QString IntervalRange::toString() const
{   if ( _items.size() == 0)
        return sUNDEF;
    QString names;
    for(const SPInterval& it: _items) {
        if ( names != "")
            names += ";";
        names += it->name();
    }
    return names;
}

void IntervalRange::clear()
{
    _items.clear();
}

ItemRange *IntervalRange::clone() const
{
    ItemRange *items = new IntervalRange();
    for(const SPInterval& it: _items) {
        items->add(it->clone());
    }
    items->interpolation(interpolation());

    return items;

}

IntervalRange &IntervalRange::operator <<(const QString &itemdef)
{
    QStringList parts = itemdef.split("|");
    if( parts.size() > 2) {
        bool ok;
        double step = 0;
        QString name = parts[0];
        double mn = parts[1].toDouble(&ok);
        if(ok) {
            double mx = parts[2].toDouble(&ok);
            if ( ok) {
                if ( parts.size() == 4) {
                    step = parts[3].toDouble(&ok);

                }
                if (ok) {
                    add(new Interval(name, {mn,mx, step}));
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
        double step = _items[0]->range().resolution();
        if ( step == 0)
            vmin += EPS8;
        QString label = QString("label_%1").arg(_items.size());
        add(new Interval(label, {vmin+step,vmax, step}));
    }
    return *this;
}

qint32 IntervalRange::gotoIndex(qint32 index, qint32 step) const
{
    if ( index + step >= _items.size())
        return _items.size();
    if ( index + step < 0)
        return 0;
    return index + step;
}

void IntervalRange::addRange(const ItemRange &range)
{
    ItemRange::addRange(range);
    _interpolation = static_cast<const IntervalRange&>(range).interpolation();
}

SPDomainItem IntervalRange::valueAt(quint32 index, Range *rng){
    if ( rng && hasType(rng->valueType(), itNUMERICITEM)){
        IntervalRange *idrange = static_cast<IntervalRange *>(rng);
        if ( index < idrange->count()){
            return idrange->item(index);
        }
    }
    return SPDomainItem();
}



