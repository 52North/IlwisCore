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

bool IndexedIdentifierRange::contains(const QString& item , bool ) const{
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
    QString s =  _start->name();
    if ( index < count())
        return s + QString::number(index);
    return sUNDEF;
}
quint32 IndexedIdentifierRange::count() const{
    return _count;
}

void IndexedIdentifierRange::count(quint32 nr)
{
    _count = nr;
}

bool IndexedIdentifierRange::isContinuous() const
{
    return false;
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
    for(quint32 i=0; i < _count; ++i) {
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

bool NamedIdentifierRange::contains(const QString& name, bool ) const
{
    if ( name == "" || name == "?")
        return false;

    SPDomainItem it = item(name);
    return it && it->isValid();
}


bool NamedIdentifierRange::isValid() const
{
    return _byName.size() != 0;
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
    _byName[thing->name()] = nid;
    if ( nid->raw() == iUNDEF)
        nid->_raw = _byRaw.size();
    _byRaw[nid->_raw] = nid;

    return ;
}

bool NamedIdentifierRange::operator==(const ItemRange &range) const
{
    if ( _byName.size() != range.count())
        return false;

    for(int i = 0; i < _byName.size(); ++i) {
        if (!contains(range.item(i)->name()))
            return false;
    }

    return true;
}

void NamedIdentifierRange::remove(const QString &name)
{
    auto iter = _byName.find(name);
    if ( iter == _byName.end())
        return;
    quint32 iraw = (*iter).second->raw();
    _byName.erase(name);
    _byRaw.erase(iraw);

    return ;
}

QString NamedIdentifierRange::value(quint32 index) const
{
    SPNamedIdentifier it = item(index).dynamicCast<NamedIdentifier>();
    if (!it)
        return sUNDEF;

    return it->name();
}

SPDomainItem NamedIdentifierRange::item(quint32 iraw) const {
    auto iter = _byRaw.find(iraw);
    if ( iter != _byRaw.end())
        return (*iter).second;
    return SPDomainItem();
}

SPDomainItem NamedIdentifierRange::item(const QString& nam) const{
    auto iter = _byName.find(nam);
    if (iter != _byName.end())
        return (*iter).second;

    return SPDomainItem();
}

quint32 NamedIdentifierRange::count() const
{
    return _byName.size();
}

Range *NamedIdentifierRange::clone() const
{
    NamedIdentifierRange *nir = new NamedIdentifierRange();
    for(auto kvp: _byName) {
        nir->add( kvp.second->clone());
    }
    return nir;
}


NamedIdentifierRange &NamedIdentifierRange::operator <<(const QString &itemdef)
{
    this->add(new NamedIdentifier(itemdef));

    return *this;
}

QString NamedIdentifierRange::toString() const {
    QString res;
    for(auto item :_byName) {
        if (res!= "")
            res += ",";
        res += item.second->name();
    }
    return res;
}

bool NamedIdentifierRange::isContinuous() const
{
    return false;
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
