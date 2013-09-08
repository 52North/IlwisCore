#include <QString>
#include <QSharedPointer>
#include "kernel.h"
#include "range.h"
#include "domainitem.h"
#include "itemrange.h"
#include "identifieritem.h"
#include "identifierrange.h"


using namespace Ilwis;

IndexedIdentifier::IndexedIdentifier() : _index(0),_prefix(""),_count(iUNDEF)
{
}

IndexedIdentifier::IndexedIdentifier(const QString &label, quint32 ind, qint32 cnt)
{
    _prefix = label;
    _index = ind;
    if ( cnt != iUNDEF)
        _count = cnt;
}


QString IndexedIdentifier::name(quint32 ind) const
{
    if ( _prefix != "")
        return QString("%1_%2").arg(_prefix).arg(_index + ind);
    return QString::number(_index + ind);
}

quint32 IndexedIdentifier::index() const
{
    return _index;
}

void IndexedIdentifier::setIndex(quint32 ind)
{
    _index = ind;
}

QString IndexedIdentifier::prefix() const
{
    return _prefix;
}

void IndexedIdentifier::setPrefix(const QString &pf)
{
    _prefix = pf;
}

bool IndexedIdentifier::operator==(const IndexedIdentifier &item) const
{
    return _prefix == item._prefix && _index == item._index;
}

DomainItem *IndexedIdentifier::clone() const
{
    return new IndexedIdentifier(_prefix, _index, _count);
}

bool IndexedIdentifier::isValid() const{
    return true;
}

//QString IndexedIdentifier::itemType() const
//{
//    return "IndexedIdentifier";
//}

ItemRange *IndexedIdentifier::createRange()
{
    return new IndexedIdentifierRange();
}

IlwisTypes IndexedIdentifier::valueTypeS()
{
    return itINDEXEDITEM;
}

IlwisTypes IndexedIdentifier::valueType() const
{
    return valueTypeS();
}

//---------------------------------------------------------------
NamedIdentifier::NamedIdentifier() : _name(sUNDEF)
{
}

NamedIdentifier::NamedIdentifier(const QString &nm) : _name(nm)
{
}

QString NamedIdentifier::name(quint32 ) const
{
    return _name;
}

void NamedIdentifier::setName(const QString &n)
{
    _name = n;
}

bool NamedIdentifier::operator ==(const NamedIdentifier &item) const
{
    return _name == item.name();
}

IlwisTypes NamedIdentifier::valueType() const
{
    return valueTypeS();
}

NamedIdentifierRange *NamedIdentifier::createRange()
{
    return new NamedIdentifierRange();
}

IlwisTypes NamedIdentifier::valueTypeS()
{
    return itNAMEDITEM;
}



DomainItem *NamedIdentifier::clone() const
{
    return new NamedIdentifier(_name);
}

bool NamedIdentifier::isValid() const {
    return _name != sUNDEF && _name != "";
}


