#include <QString>
#include <typeinfo>
#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "domainitem.h"
#include "itemrange.h"
#include "identifieritem.h"
#include "domain.h"
#include "itemdomain.h"
#include "thematicitem.h"
#include "identifierrange.h"

using namespace Ilwis;

IndexedIdentifierRange::IndexedIdentifierRange() : _count(0)
{
    _vt = itINDEXEDITEM;
}

bool IndexedIdentifierRange::contains(const QString& item ) const{
    if (_start->prefix() != "") {
        if ( _start->prefix() != item.left(_start->prefix().size()))
            return false;
    }
    int index = item.indexOf("_");
    if ( index != -1) {
        QString num = item.mid(index);
        bool ok;
        quint32 itemIndex = num.toULong(&ok);
        return ok && itemIndex < _count;
    }
    return false;
}

bool IndexedIdentifierRange::isValid() const{
    return _count != 0;
}

bool IndexedIdentifierRange::operator==(const IndexedIdentifierRange& range){
    return _start == range._start;
}

QString IndexedIdentifierRange::value(quint32 index) const{
    return _start->name(index);
}
quint32 IndexedIdentifierRange::count() const{
    return _count;
}

void IndexedIdentifierRange::count(quint32 nr)
{
    _count = nr;
}

SPDomainItem IndexedIdentifierRange::item(const QString& nam) const{
    return SPDomainItem();
}

Range *IndexedIdentifierRange::clone() const
{
    IndexedIdentifierRange *irange = new IndexedIdentifierRange();
    irange->_count = _count;
    irange->add(_start->clone());

    return irange;

}

SPDomainItem IndexedIdentifierRange::item(quint32 index) const{
    if ( index >= _count)
        return SPDomainItem();
    return SPDomainItem( new IndexedIdentifier(_start->prefix(), index));
}

void IndexedIdentifierRange::add(DomainItem *thing)
{
    if (!thing->isValid())
        return;

    IndexedIdentifier *ii = dynamic_cast<IndexedIdentifier *>(thing);
    _start = SPIndexedIdentifier(ii);
    _count = ii->_count;

    return;
}

QString IndexedIdentifierRange::toString() const
{
    QString res;
    for(quint32 i=_start->index(); i < _start->index() + _count; ++i) {
        if ( res!= "")
            res = res +  ",";
        if ( _start->prefix() != "")
            res += _start->prefix() + "_";
        res += QString::number(i);
    }
    return res;
}

void IndexedIdentifierRange::remove(const QString& item)
{
}

//-------------------------------------------------------------------------
NamedIdentifierRange::NamedIdentifierRange()
{
        _vt = itNAMEDITEM;
}

NamedIdentifierRange::~NamedIdentifierRange()
{
}

bool NamedIdentifierRange::contains(const QString& name) const
{
    if ( name == "" || name == "?")
        return false;

    SPDomainItem it = item(name);
    return it && it->isValid();
}


bool NamedIdentifierRange::isValid() const
{
    return _items.size() != 0;
}


void NamedIdentifierRange::add(DomainItem *thing)
{
    if (!thing)
        return ;

    if (!thing->isValid()) {
        delete thing;
        return;
    }

    if ( item(thing->name()) != 0) {
        delete thing;
        return ;
    }

    SPNamedIdentifier nid(static_cast<NamedIdentifier *>(thing));
    _items[thing->name()] = nid;
    _indexes.push_back(nid);

    return ;
}

bool NamedIdentifierRange::operator==(const ItemRange &range) const
{
    if ( _items.count() != range.count())
        return false;

    for(int i = 0; i < _indexes.count(); ++i) {
        if (!contains(range.item(i)->name()))
            return false;
    }

    return true;
}

void NamedIdentifierRange::remove(const QString &name)
{
    SPDomainItem it = item(name);
    if (!it)
        return ;

    _items.remove(name);
    int ind = _indexes.indexOf(it.dynamicCast<NamedIdentifier>());
    _indexes.remove(ind);

    return ;
}

QString NamedIdentifierRange::value(quint32 index) const
{
    SPNamedIdentifier it = item(index).dynamicCast<NamedIdentifier>();
    if (!it)
        return sUNDEF;

    return it->name();
}

SPDomainItem NamedIdentifierRange::item(quint32 index) const {
    if ( index <= 0 || index >= count())
        return SPDomainItem();

    return _indexes[index];
}

SPDomainItem NamedIdentifierRange::item(const QString& nam) const{
    auto iter = _items.find(nam);
    if (iter != _items.end())
        return iter.value();

    return SPDomainItem();
}

quint32 NamedIdentifierRange::count() const
{
    return _items.size();
}

Range *NamedIdentifierRange::clone() const
{
    NamedIdentifierRange *nir = new NamedIdentifierRange();
    for(const auto &iter: _items) {
        nir->add( (*iter).clone());
    }
    NamedIdentifierRange pp;
    return nir;
}


NamedIdentifierRange &NamedIdentifierRange::operator <<(const QString &itemdef)
{
    this->add(new NamedIdentifier(itemdef));

    return *this;
}

QString NamedIdentifierRange::toString() const {
    QString res;
    foreach(SPNamedIdentifier item, _indexes) {
        if (res!= "")
            res += ",";
        res += item->name();
    }
    return res;
}

//---------------------------------------------------------
ThematicRange::ThematicRange()
{
  _vt = itTHEMATICITEM;
}

ThematicRange &ThematicRange::operator<<(const QString &itemdef)
{
    add(new ThematicItem(itemdef));
    return *this;
}
