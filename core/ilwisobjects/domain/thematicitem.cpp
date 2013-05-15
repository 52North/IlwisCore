#include <QString>
#include <QSharedPointer>
#include "kernel.h"
#include "identity.h"
#include "ilwisdata.h"
#include "ilwisobject.h"
#include "domain.h"
#include "itemdomain.h"
#include "domainitem.h"
#include "identifieritem.h"
#include "thematicitem.h"

using namespace Ilwis;

inline bool operator == (const Ilwis::ThematicItem& item1, const Ilwis::ThematicItem& item2) {
    return item1.name() == item2.name();
}

ThematicItem::ThematicItem()
{
    setCode(sUNDEF);
    setDescription(sUNDEF);
}

ThematicItem::ThematicItem(const QString &nm, const QString &cde, const QString &descr) : NamedIdentifier(nm),_description(descr), _code(cde)
{
}

QString ThematicItem::itemType() const
{
    return "ThematicItem";
}

QString ThematicItem::description() const
{
    return _description;
}

void ThematicItem::setDescription(const QString &desc)
{
    _description = desc;
}

void ThematicItem::setCode(const QString &code)
{
    _code = code;
}


void ThematicItem::store(QDataStream &s, const SerializationOptions &opt) const
{
    NamedIdentifier::store(s, opt) ;
    s << _code << _description;
}

void ThematicItem::load(QDataStream &s)
{
}



