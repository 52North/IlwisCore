#include <QString>
#include <typeinfo>
#include "kernel.h"
#include "range.h"
#include "domainitem.h"
#include "itemrange.h"
#include "identifieritem.h"
#include "identifierrange.h"

using namespace Ilwis;

IndexedIdentifierRange::IndexedIdentifierRange() : _count(0)
{
}

bool IndexedIdentifierRange::contains(const QString& ) const{
    // TODO implement contains
    return false;
}

bool IndexedIdentifierRange::isValid() const{
    return _count != 0;
}

bool IndexedIdentifierRange::operator==(const IndexedIdentifierRange& range){
    return _start == range._start;
}

QString IndexedIdentifierRange::value(quint32 index) const{
    return _start.name(index);
}
quint32 IndexedIdentifierRange::count() const{
    return _count;
}

DomainItem *IndexedIdentifierRange::item(const QString& nam) const{
    return 0;
}

Range *IndexedIdentifierRange::clone() const
{
    //TODO
    return 0;
}

DomainItem *IndexedIdentifierRange::item(qint64 ) const{
    return 0;
}

void IndexedIdentifierRange::add(DomainItem *thing)
{
    if (!thing->isValid())
        return;

    _start = *static_cast<IndexedIdentifier *>(thing);

    delete thing;

    return;
}

void IndexedIdentifierRange::remove(const QString& )
{
}

//-------------------------------------------------------------------------
NamedIdentifierRange::NamedIdentifierRange()
{
}

NamedIdentifierRange::~NamedIdentifierRange()
{
    qDeleteAll(_indexes);
}

bool NamedIdentifierRange::contains(const QString& name) const
{
    if ( name == "" || name == "?")
        return false;

    DomainItem *it = item(name);
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


    _items[thing->name()] = static_cast<NamedIdentifier *>(thing);
    _indexes.push_back(static_cast<NamedIdentifier *>(thing));

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
    DomainItem *it = item(name);
    if (!it)
        return ;

    _items.remove(name);
    int ind = _indexes.indexOf(static_cast<NamedIdentifier *>(it));
    _indexes.remove(ind);
    delete it;

    return ;
}

QString NamedIdentifierRange::value(quint32 index) const
{
    NamedIdentifier *it = static_cast<NamedIdentifier *>(item(index));
    if (!it)
        return sUNDEF;

    return it->name();
}

DomainItem *NamedIdentifierRange::item(quint32 index) const {
    if ( index <= 0 || index >= count())
        return 0;

    return _indexes[index];
}

DomainItem *NamedIdentifierRange::item(const QString& nam) const{
    auto iter = _items.find(nam);
    if (iter != _items.end())
        return iter.value();

    return 0;
}

quint32 NamedIdentifierRange::count() const
{
    return _items.size();
}

Range *NamedIdentifierRange::clone() const
{
    //TODO
    return 0;
}


NamedIdentifierRange &NamedIdentifierRange::operator <<(const QString &item)
{
   this->add(new NamedIdentifier(item));

    return *this;
}

QString NamedIdentifierRange::toString() const {
    QString res;
    foreach(NamedIdentifier* item, _indexes) {
        if (res!= "")
            res += ",";
        res += item->name();
    }
    return res;
}





