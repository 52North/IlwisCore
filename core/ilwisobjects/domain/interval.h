#ifndef NUMERICITEM_H
#define NUMERICITEM_H

#include "thematicitem.h"

namespace Ilwis {

class  IntervalRange;

class KERNELSHARED_EXPORT Interval : public ThematicItem
{
public:
    friend class IntervalRange;

    /**
     * Constructor for an empty Interval
     */
    Interval();

    /**
     * Constructor for a Interval with range vr.<br>
     * vr must be valid. vr will be set as the range of this item.
     *
     * @param vr the NumericRange of this item
     */
    Interval(const QString label, const NumericRange &vr);

    virtual ~Interval() {}

    //@override
    bool isValid() const;
     bool isOrdered() const;

    //@override
    DomainItem *clone() const;

    //@override
    IlwisTypes valueType() const;

    /**
     * Query for the range of this Interval.<br>
     * Returns the numeric range of this item, if the range is not set it will return null
     *
     * @return the NumericRange of this
     */
    NumericRange range() const;

    /**
     * Sets a new NumericRange on this Interval.<br>
     * The new range must be valid.
     * @param valuerange the new range
     */
    void range(const NumericRange &valuerange);

    /**
     * Clears the item range on this item.
     *\sa NumericRange
     */
    void clear();

    /**
     * Creates a empty new NumericItemRange and returns this
     * \sa NumericItemRange
     * @return a empty new NumericItemRange
     */
    static Ilwis::IntervalRange *createRange();


    //@override
    static IlwisTypes valueTypeS();

private:
    NumericRange _valuerange;
};

struct CompareNumericItem {
    bool operator()(const Interval& it1, const Interval& it2)     {
        return it1.range() < it2.range();
    }
};

typedef QSharedPointer<Interval> SPInterval;

}

#endif // NUMERICITEM_H
