#include <QString>
#include "kernel.h"
#include "range.h"

using namespace Ilwis;

Range::Range()
{
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
