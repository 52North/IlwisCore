#include <QUrl>
#include "module.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "mastercatalog.h"

using namespace Ilwis;

Domain::Domain(): _strict(true)
{
}

Domain::Domain(const Resource& resource) : IlwisObject(resource), _strict(true)
{
}

bool Ilwis::Domain::isStrict() const
{
    return _strict;
}

void Ilwis::Domain::setStrict(bool yesno)
{
    _strict = yesno;
}

IDomain Domain::parent() const
{
    return _parentDomain;
}

void Domain::setParent(const IDomain &dm)
{
    _parentDomain = dm;
}

bool Domain::addChildDomain(quint64 idchild)
{
    _childDomains.insert(idchild);
}

void Domain::removeChildDomain(quint64 idchild)
{
    _childDomains.erase(idchild);
}

IlwisTypes Domain::ilwType(const QString &value) {
    std::set<QString> booleans ={ "yes","no","true","false"};
    if ( booleans.find(value.toLower()) != booleans.end())
        return itBOOL;

    bool ok;
    ushort vu = value.toUShort(&ok);
    if ( ok && vu < 255)
        return itUINT8;
    if ( ok)
        return itUINT16;
    short vs = value.toShort(&ok);
    if ( ok && vs > -128 && vs < 128)
        return itINT8;
    if ( ok)
       return itINT16;
    value.toULong(&ok);
    if ( ok)
        return itUINT32;
    value.toLong(&ok);
    if ( ok)
       return itINT32;
    value.toLongLong(&ok);
    if ( ok)
        return itINT64;
    value.toDouble(&ok);
    if ( ok)
       return itDOUBLE;

    return IlwisObject::findType(value);
}

IlwisTypes Domain::ilwType(const QVariant &v)
{
    QVariant::Type ty = v.type();
    switch(ty) {
        case QVariant::Bool:
            return itUINT8; break;
        case QVariant::Double:
            return itDOUBLE; break;
        case QVariant::Int:
          return itINT32; break;
        case QVariant::LongLong:
        case QVariant::ULongLong:
            return itINT64; break;
        case QVariant::UInt:
            return itUINT32; break;
        case QVariant::String:
              return itSTRING; break;
        case QVariant::Time:
            return itTIME; break;
        default:
            return itUNKNOWN;

    }
    return itUNKNOWN;
}


