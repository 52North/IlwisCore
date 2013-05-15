#include <QUrl>
#include "module.h"
#include "kernel.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "mastercatalog.h"

using namespace Ilwis;

Domain::Domain()
{
}

Domain::Domain(const Resource& res) : IlwisObject(res)
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


