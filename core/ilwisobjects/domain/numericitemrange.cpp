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

SPDomainItem NumericItemRange::item(const QString &name) const
{
    for(const SPNumericItem& it: _items) {
        if ( it->name() == name)
            return it;
    }
    return SPDomainItem();
}

double NumericItemRange::index(double v) const
{
    if (!isValid())
        return rUNDEF;

    quint32 mark1=iUNDEF, mark2=iUNDEF;
    for(quint32 ind = 0; ind < _items.size(); ++ind) {
        const NumericRange& rng = _items[ind]->range();
        if ( v >= rng.min()){
            mark1 = ind;
        }
        if ( v < rng.max()){
            mark2 = ind;
        }
        if ( mark1 != rUNDEF && mark2 != rUNDEF) {
            if ( isContinous())
                return ind;
            return ind + (v - rng.min()) / rng.distance();
        }
    }
    return rUNDEF;
}

bool NumericItemRange::contains(const QString &name) const
{
    for(const SPNumericItem& it: _items) {
        if ( it->name() == name)
            return true;
    }
    return false;
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
    for(auto iter = _items.begin(); iter != _items.begin(); ++iter) {
        if ( (*iter)->range() > nitem->range()) {
            _items.insert(iter,1, nitem );
            return;
        }
    }
    if ( _items.size() == 0) // no overlapping items allowed; so the only case that is legal here is the first
        _items.push_back(nitem);

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
                }
            }
        }
    }
    return *this;
}

bool NumericItemRange::isContinous() const
{
    return _interpolation != "";
}

void NumericItemRange::setInterpolation(const QString &ip)
{
    _interpolation = ip;
}

