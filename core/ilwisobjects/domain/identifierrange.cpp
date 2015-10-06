#include <QString>
#include <typeinfo>
#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "domain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "itemrange.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "identifierrange.h"

using namespace Ilwis;

IndexedIdentifierRange::IndexedIdentifierRange() : _count(0)
{
}

IndexedIdentifierRange::IndexedIdentifierRange(const QString &prefix, quint32 count, quint32 start)
{
    add(new IndexedIdentifier(prefix,0, count));
}

bool IndexedIdentifierRange::contains(const QVariant &item , bool ) const{
    if (_start->prefix() != "") {
        if ( _start->prefix() != item.toString().left(_start->prefix().size()))
            return false;
    }
    int index = item.toString().indexOf("_");
    if ( index != -1) {
        QString num = item.toString().mid(index + 1);
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

QVariant IndexedIdentifierRange::impliedValue(const QVariant &v) const{
    bool ok;
    quint32 index = v.toUInt(&ok);
    if (!ok){
        ERROR2(ERR_COULD_NOT_CONVERT_2,v.toString(), "raw value");
        return sUNDEF;
    }
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

bool IndexedIdentifierRange::alignWithParent(const IDomain &dom)
{
    return false;
}

SPDomainItem IndexedIdentifierRange::item(const QString& nam) const{
    return SPDomainItem();
}

SPDomainItem IndexedIdentifierRange::itemByOrder(quint32 index) const
{
    return item(index);
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
    auto iter = _existingItems.find(index);
    if ( iter != _existingItems.end())
        return (*iter).second;
    SPDomainItem newItem( new IndexedIdentifier(_start->prefix(), index, _count));
    const_cast<IndexedIdentifierRange *>(this)->_existingItems[index] = newItem;
    return newItem;
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

void IndexedIdentifierRange::add(SPDomainItem item)
{
    if (!item->isValid())
        return;

    SPIndexedIdentifier ii = item.staticCast<IndexedIdentifier>();
    _start = ii;
    _count = ii->_count;

    return;
}

void IndexedIdentifierRange::add(const QVariant &v)
{
    if ( contains(v))
        return;
    ++_count;
}

QString IndexedIdentifierRange::toString() const
{
    QString resource;
    for(quint32 i=0; i < _count; ++i) {
        if ( resource!= "")
            resource = resource +  "|";
        if ( _start->prefix() != "")
            resource += _start->prefix() + "_";
        resource += QString::number(i);
    }
    return "indexedidentifierrange:" + resource;
}

void IndexedIdentifierRange::remove(const QString& item)
{
}

qint32 IndexedIdentifierRange::gotoIndex(qint32 index, qint32 step) const
{
    if ( index + step >= _count)
        return _count;
    if ( index + step < 0)
        return 0;
    return index + step;
}

IlwisTypes IndexedIdentifierRange::valueType() const
{
    return itINDEXEDITEM;
}

IndexedIdentifierRange *IndexedIdentifierRange::merge(const QSharedPointer<IndexedIdentifierRange> &nr1, const QSharedPointer<IndexedIdentifierRange> &nr2,RenumberMap *rnm)
{
    SPIndexedIdentifier index1 = nr1->item(0).staticCast<IndexedIdentifier>();
    SPIndexedIdentifier index2 = nr2->item(0).staticCast<IndexedIdentifier>();
    if ( index1->prefix() != index2->prefix())
        return 0;
    return new IndexedIdentifierRange(index1->prefix(), nr1->count() + nr2->count());

}

void IndexedIdentifierRange::clear()
{
    _count = 0;
    //_start.reset(0);
}

void IndexedIdentifierRange::store(QDataStream &stream)
{
    stream << _count;
    stream << _start->name() << _start->prefix();
}

void IndexedIdentifierRange::load(QDataStream &stream)
{
    stream >> _count;
    QString name, prefix;
    stream >> name >> prefix;
    _start.reset(new IndexedIdentifier(name));
    _start->setPrefix(prefix);
}

SPDomainItem IndexedIdentifierRange::valueAt(quint32 &index, const Range *rng){
    if ( rng && hasType(rng->valueType(), itINDEXEDITEM)){
        const IndexedIdentifierRange *idrange = static_cast<const IndexedIdentifierRange *>(rng);
        if ( index < idrange->count()){
            return idrange->item(index);
        }
    }
    index =iUNDEF;
    return SPDomainItem();
}

QString IndexedIdentifierRange::valueAsString(quint32 &index, const Range *rng)
{
    SPDomainItem item = IndexedIdentifierRange::valueAt(index, rng);
    return item->name();
}

//-------------------------------------------------------------------------
NamedIdentifierRange::NamedIdentifierRange()
{
}

NamedIdentifierRange::~NamedIdentifierRange()
{
}

bool NamedIdentifierRange::contains(const QVariant &v, bool ) const
{
    QString name = v.toString();
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
        nid->_raw = _byOrder.size();
    if ( nid->_raw < _byRaw.size())
        _byRaw[nid->_raw] = nid;
    else{
        _byRaw.resize(nid->_raw + 100);
        _byRaw[nid->_raw] = nid;
    }
    _byOrder.push_back(nid);

    return ;
}

void NamedIdentifierRange::add(SPDomainItem item)
{
    if (item.isNull() || !item->isValid() || !hasType(item->valueType(),itTHEMATICITEM | itNAMEDITEM )) {
        return;
    }

    auto iter = _byName.find(item->name());
    if ( iter != _byName.end())
        return;

    SPNamedIdentifier nid = item.staticCast<NamedIdentifier>();
    _byName[item->name()] = nid;
    if ( nid->raw() == iUNDEF)
        nid->_raw = _byOrder.size();
    if ( nid->_raw < _byRaw.size())
        _byRaw[nid->_raw] = nid;
    else{
        _byRaw.resize(nid->_raw + 100);
        _byRaw[nid->_raw] = nid;
    }
    _byOrder.push_back(nid);
}

void NamedIdentifierRange::add(const QVariant &v)
{
    if (contains(v))
        return;
    if ( v.isValid() && v.toString() != "")
        add( new NamedIdentifier(v.toString()));
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
    _byRaw.erase(_byRaw.begin() + iraw);
    for(int i=0; i < count(); ++i) {
        if ( _byOrder[i]->name() == name){
            _byOrder.erase(_byOrder.begin() + i);
            break;
        }
    }

    return ;
}

QVariant NamedIdentifierRange::impliedValue(const QVariant& v) const
{
    bool ok;
    quint32 index = v.toUInt(&ok);
    if (!ok){
        QString typName = v.typeName();
        if ( typName == "QString"){
            SPNamedIdentifier it = item(v.toString()).staticCast<NamedIdentifier>();
            if (it)
                return it->raw();
        }
        ERROR2(ERR_COULD_NOT_CONVERT_2,v.toString(), "raw value");
        return QVariant();
    }
    SPNamedIdentifier it = item(index).staticCast<NamedIdentifier>();
    if (!it)
        return QVariant();

    return it->name();
}

SPDomainItem NamedIdentifierRange::item(quint32 iraw) const {
    if ( isNumericalUndef(iraw) || iraw >= _byRaw.size())
        return SPDomainItem();

    return _byRaw.at(iraw);

}

SPDomainItem NamedIdentifierRange::item(const QString& nam) const{
    auto iter = _byName.find(nam);
    if (iter != _byName.end())
        return (*iter).second;

    return SPDomainItem();
}

SPDomainItem NamedIdentifierRange::itemByOrder(quint32 index) const
{
    if (index < count())
        return _byOrder[index];
    return SPDomainItem();
}




quint32 NamedIdentifierRange::count() const
{
    return _byName.size();
}

IlwisTypes NamedIdentifierRange::valueType() const
{
    return itNAMEDITEM;
}

void NamedIdentifierRange::clear()
{
    _byName.clear();
    _byOrder.clear();
    _byRaw.clear();
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
    if (!contains(itemdef))
        this->add(new NamedIdentifier(itemdef));

    return *this;
}

QString NamedIdentifierRange::toString() const {
    QString resource = asString();
    return "namedidentifierrange:"+ resource;
}

QString NamedIdentifierRange::asString() const {
    QString resource;
    for(auto item :_byName) {
        if (resource!= "")
            resource += "|";
        resource += item.second->name();
    }
    return resource;
}

bool NamedIdentifierRange::isContinuous() const
{
    return false;
}

void NamedIdentifierRange::store(QDataStream &stream)
{
    int size = _byName.size();
    stream << size;
    for(const auto& item : _byRaw) {
        if ( item == 0) // not a bug; there are always at the end undefined raws, so we can stop now
            break;
        stream <<  item->raw() << item->name();
    }
}

void NamedIdentifierRange::load(QDataStream &stream)
{
    int numberOfItems;
    stream >> numberOfItems;
    for(int i=0; i < numberOfItems; ++i){
        quint32 raw;
        QString name;
        stream >> raw >> name;
        add(new NamedIdentifier(name, raw));
    }

}

bool NamedIdentifierRange::alignWithParent(const IDomain &dom)
{
    INamedIdDomain numdom = dom.as<NamedIdDomain>();
    if ( !numdom.isValid())
        return false;

    std::map<QString, DomainItem *> parentItems;
    for(DomainItem *item : numdom) {
        parentItems[item->name()] = item;
    }

    for(int i=0; i < _byOrder.size(); ++i) {
        auto iter = parentItems.find(_byOrder[i]->name());
        if ( iter == parentItems.end()){
            return ERROR2(ERR_ILLEGAL_VALUE_2, TR("item in child domain"),_byOrder[i]->name());
        }
        _byOrder[i] = SPNamedIdentifier(static_cast<NamedIdentifier *>((*iter).second->clone()));
    }
    //TODO: at this moment we should start checking all item in the master catalog using this domain/range.
    _byRaw.clear();
    _byName.clear();
    for(SPNamedIdentifier item : _byOrder){
        if ( item->_raw < _byRaw.size())
            _byRaw[item->_raw] = item;
        else{
            _byRaw.resize(item->_raw + 100);
            _byRaw[item->_raw] = item;
        }
        _byName[item->name()] = item;
    }
    return true;

}

qint32 NamedIdentifierRange::gotoIndex(qint32 index, qint32 step) const
{
    if ( index + step >= _byOrder.size())
        return _byOrder.size();
    if ( index + step < 0)
        return 0;
    return index + step;
}

NamedIdentifierRange *NamedIdentifierRange::merge(const QSharedPointer<NamedIdentifierRange> &nr1, const QSharedPointer<NamedIdentifierRange> &nr2, RenumberMap *rnm)
{
    NamedIdentifierRange *newRange = new NamedIdentifierRange();
    newRange->addRange(*nr1.data());
    newRange->addRange(*nr2.data());

    addItems<NamedIdentifier>(newRange, nr1, nr2, rnm);

    return newRange;
}

SPDomainItem NamedIdentifierRange::valueAt(quint32& index, const Range *rng){
    if ( rng && hasType(rng->valueType(), itNAMEDITEM)){
        const NamedIdentifierRange *idrange = static_cast<const NamedIdentifierRange *>(rng);
        if ( index < idrange->count()){
            return idrange->item(index);
        }
    }
    index = iUNDEF;
    return SPDomainItem();
}

QString NamedIdentifierRange::valueAsString(quint32 &index, const Range *rng)
{
    SPDomainItem item = IndexedIdentifierRange::valueAt(index, rng);
    return item->name();
}

//---------------------------------------------------------
ThematicRange::ThematicRange()
{
}

ThematicRange *ThematicRange::merge(const QSharedPointer<ThematicRange> &nr1, const QSharedPointer<ThematicRange> &nr2,RenumberMap *renumberer)
{
    ThematicRange *newRange = new ThematicRange();
    newRange->addRange(*nr1.data());
    newRange->addRange(*nr2.data());

    addItems<ThematicItem>(newRange, nr1, nr2, renumberer);

    return newRange;
}

ThematicRange &ThematicRange::operator<<(const QString &itemdef)
{
    QStringList parts = itemdef.split("|");
    add(new ThematicItem(parts));
    return *this;
}

Range *ThematicRange::clone() const
{
    ThematicRange *tr = new ThematicRange();
    for(auto kvp: _byName) {
        tr->add( kvp.second->clone());
    }
    return tr;
}

SPDomainItem ThematicRange::valueAt(quint32& index, const Range *rng){
    if ( rng && hasType(rng->valueType(), itTHEMATICITEM)){
        const ThematicRange *idrange = static_cast<const ThematicRange *>(rng);
        if ( index < idrange->count()){
            return idrange->item(index);
        }
    }
    index = iUNDEF;
    return SPDomainItem();
}

QString ThematicRange::valueAsString(quint32 &index, const Range *rng)
{
    SPDomainItem item = IndexedIdentifierRange::valueAt(index, rng);
    return item->name();
}


IlwisTypes ThematicRange::valueType() const
{
    return itTHEMATICITEM;
}

void ThematicRange::store(QDataStream &stream)
{
    int size = _byName.size();
    stream << size;
    for(const auto& item : _byName) {
        stream <<  item.second->raw() << item.second->name() << item.second->as<ThematicItem>()->description() << item.second->as<ThematicItem>()->code();
    }
}

QString ThematicRange::toString() const {
    QString resource = asString();
    return "thematicrange:"+ resource;
}

void ThematicRange::load(QDataStream &stream)
{
    int numberOfItems;
    stream >> numberOfItems;
    for(int i=0; i < numberOfItems; ++i){
        quint32 raw;
        QString name, description, code;
        stream >> raw >> name >> description >> code;
        add(new ThematicItem({name, code, description}, raw));
    }
}

