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
    if ( parent().isValid())
        parent()->removeChildDomain(this->id());
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
    if ( dm.isValid())
        dm->addChildDomain(id());
    else
        _parentDomain->removeChildDomain(id());
    _parentDomain = dm;


}

void Domain::addChildDomain(quint64 idchild)
{
    auto iter = _childDomains.find(idchild);
    if ( iter != _childDomains.end())
        (*iter).second++;
    else
        _childDomains[idchild] = 1;
}

bool Domain::removeChildDomain(quint64 idchild)
{
    auto iter = _childDomains.find(idchild);
    if ( iter != _childDomains.end()) {
        (*iter).second--;
        if ( (*iter).second == 0)
            _childDomains.erase(idchild);
        return true;
    }
    return false;
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
        case QVariant::Date:
            return itDATE; break;
        case QVariant::DateTime:
            return itDATETIME;
        case QVariant::List:
            return itCOLLECTION;
        default:
            return itUNKNOWN;

    }
    return itUNKNOWN;
}


