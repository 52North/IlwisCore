#include <QDateTime>
#include <time.h>
#include "kernel.h"
#include "juliantime.h"
#include "aggregationtime.h"

using namespace Ilwis;

AggregationTime::AggregationTime(const Time& time)
    : _time(time)
{
}

int AggregationTime::dekadFromDate(const Time& date) {
    if (date.isValid()) {
        int dix = (date.get(Time::tpDAYOFMONTH) - 1) / 10;
        int mix = date.get(Time::tpMONTH) - 1;
        return mix * 3 + dix;
    }
    return -1;
}

int AggregationTime::get(AggregationTimePart part) const {
    if (part == tpDekad)
        return AggregationTime::dekadFromDate(_time);
    return -1;
}
