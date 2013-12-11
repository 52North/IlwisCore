#include <QString>
#include <QSharedPointer>
#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "domain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "itemrange.h"
#include "identifieritem.h"
#include "identifierrange.h"


using namespace Ilwis;

IndexedIdentifier::IndexedIdentifier() : _prefix(""),_count(iUNDEF)
{
}

IndexedIdentifier::IndexedIdentifier(const QString &label, quint32 ind, qint32 cnt)
{
    _prefix = label;
    _raw = ind;
    if ( cnt != iUNDEF)
        _count = cnt;
}


QString IndexedIdentifier::name() const
{
    if ( _raw == iUNDEF)
        return _prefix;

    if ( _prefix != "")
        return QString("%1_%2").arg(_prefix).arg(_raw);
    return QString::number(_raw);
}

quint32 IndexedIdentifier::raw() const
{
    return _raw;
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
    return _prefix == item._prefix && _raw == item._raw;
}

DomainItem *IndexedIdentifier::clone() const
{
    return new IndexedIdentifier(_prefix, _raw, _count);
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
    return new IndexedIdentifierRange("",0);
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

NamedIdentifier::NamedIdentifier(const QString &nm, quint32 rawvalue) : DomainItem(rawvalue), _name(nm)
{
}

QString NamedIdentifier::name() const
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
  NamedIdentifier *nid = new NamedIdentifier(_name);
  nid->_raw = _raw;
  return nid;
}

bool NamedIdentifier::isValid() const {
    return _name != sUNDEF && _name != "";
}


