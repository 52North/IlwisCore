#include <QString>
#include <QSharedPointer>
#include "kernel.h"
#include "identity.h"
#include "ilwisdata.h"
#include "ilwisobject.h"
#include "domainitem.h"
#include "domain.h"
#include "itemdomain.h"
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

//ThematicItem::ThematicItem(const QString &nm, const QString &cde, const QString &descr) : NamedIdentifier(nm),_description(descr), _code(cde)
//{
//}

ThematicItem::ThematicItem(const QStringList& parts) {
    if ( parts.size() > 0) {
        setName(parts[0]);
        if ( parts.size() > 1)
            setCode(parts[1]);
        if ( parts.size() == 3) {
            QString desc = parts[2];
            desc.remove("\"");
            setDescription(desc);
        }
    }
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

DomainItem *ThematicItem::clone() const
{
    return new ThematicItem({name(), _code, _description});
}

IlwisTypes ThematicItem::valueTypeS()
{
    return itTHEMATICITEM;
}

IlwisTypes ThematicItem::valueType() const
{
    return valueTypeS();
}





