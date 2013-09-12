#include <QString>
#include "kernel.h"
#include "range.h"

using namespace Ilwis;

Range::Range() : _vt(itUNKNOWN)
{
}

IlwisTypes Range::valueType() const {

    if ( _vt != itUNKNOWN)
        return _vt;

    IlwisTypes vt = determineType();

    const_cast<Range *>(this)->_vt = vt;

    return _vt;
}

bool Range::isContinuous() const
{
    return _interpolation.size() != 0;
}

QString Range::interpolation() const
{
    return _interpolation;
}

void Range::interpolation(const QString &ip)
{
    _interpolation = ip;
}
