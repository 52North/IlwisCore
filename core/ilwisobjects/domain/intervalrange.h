#ifndef NUMERICITEMRANGE_H
#define NUMERICITEMRANGE_H

#include "kernel_global.h"
#include "itemrange.h"

namespace Ilwis {
/**
 * This is a range of numeric items, for example 0-300. items can have sub ranges within this NumericItemRange.
 * for example items in 0-300 could be 0-100, 100-200 and 200-300.
 *\sa ItemRange
 */
class KERNELSHARED_EXPORT IntervalRange : public ItemRange
{
public:
    /**
     * Constructs an empty NumericItemRange
     */
    IntervalRange();

    //@override
    QVariant impliedValue(const QVariant& v) const;

    //@override
    quint32 count() const ;

    //@override
    SPDomainItem item(quint32 index) const ;

    //@override
    SPDomainItem item(const QString &def) const ;

    //@override
    SPDomainItem itemByOrder(quint32 index) const;

    /**
     * Calculates the index of a certain value v in this NumericItemRange.<br>
     * Will return iUNDEF if this item range is not valid.<br>
     * returns a double because of some interpolation.
     *
     * @param v the value
     * @return the index of this value
     */
    virtual double index(double v) const;

    //@override
    bool contains(const QVariant& name, bool inclusive = true) const ;

    //@override
    bool isValid() const;

    //@override
    void add(DomainItem *item) ;

    //@override
    void add(SPDomainItem item);
    void add(const QVariant& iditem);

    //@override
    void remove(const QString& nm) ;

    /**
     * generates a String representation of this NumericItemRange.<br>
     * the will look the following way: name;name;name; for every name in this range.
     *
     * @return a string
     */
    QString toString() const;
    void store(QDataStream& stream);
    void load(QDataStream& stream);

    /**
     * Clears all items
     */
    void clear();

    //@override
    ItemRange *clone() const;

    /**
     * override of operator<< <br>
     * this operator will now try to add the string behind it to this NumericItemRange in the form of numericitems<br>
     * it will fail at invalid strings<br>
     * <br>
     * there are 2 possible valid strings, the first case is:<br>
     * this << "1 2 3";<br>
     * which is the simple case, it will simply add a Interval of the form new Interval(1(min),2(max),3(step))<br>
     * <br>
     * and the second case is:<br>
     * this << "1"<br>
     * this requires at least 1 excisting item in the range, it will add a numeric item with as minimum the max of the last item in the current range<br>
     * and as max it will have the given value (1). step size will be the same as the resolution of the first item in the range.<br>
     * <br>
     * if anything goes wrong a ERROR1 will be thrown<br>
     * be careful to make a valid item<br>
     *
     * \sa Interval
     * @param itemdef the new item to be added
     * @return this
     */
    IntervalRange &operator <<(const QString &itemdef);

    //@override
    qint32 gotoIndex(qint32 index, qint32 step) const;
    IlwisTypes valueType() const;

    static SPDomainItem valueAt(quint32 index, const Ilwis::Range *rng);
    static QString valueAsString(quint32 index, const Range *rng);

protected:
    void addRange(const ItemRange &range);
private:
    std::vector<SPInterval> _items;
    bool validNumber(QString value) const;
    bool alignWithParent(const IDomain& dom);
};

typedef QSharedPointer<IntervalRange> SPIntervalRange;
}

#endif // NUMERICITEMRANGE_H
