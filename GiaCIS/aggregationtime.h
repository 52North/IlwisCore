#ifndef AGGREGATIONTIME_H
#define AGGREGATIONTIME_H

namespace Ilwis {

class AggregationTime
{
    AggregationTime() {}
    enum AggregationTimePart {tpDekad};
    Time _time;

public:
    AggregationTime(const Time& time);

    static int dekadFromDate(const Time& date);
    int get(AggregationTimePart part) const;
};
}

#endif // AGGREGATIONTIME_H
