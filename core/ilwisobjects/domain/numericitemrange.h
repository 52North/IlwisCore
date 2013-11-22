#ifndef NUMERICITEMRANGE_H
#define NUMERICITEMRANGE_H

namespace Ilwis {
/**
 * This is a range of numeric item, for example 0-300. items can have sub ranges within this NumericItemRange.
 * for example items in 0-300 could be 0-100, 100-200 and 200-300.
 *\sa ItemRange
 */
class KERNELSHARED_EXPORT NumericItemRange : public ItemRange
{
public:
    /**
     * Constructs an empty NumbericItemRange
     */
    NumericItemRange();

    //@override
    QString value(const QVariant& v) const;

    //@override
    quint32 count() const ;

    //@override
    SPDomainItem item(quint32 index) const ;

    //@override
    SPDomainItem item(const QString &def) const ;

    //@override
    SPDomainItem itemByOrder(quint32 index) const;

    /**
     * Calculates the index of a certain value v in this NumericItemRange
     *
     * @param v the value
     * @return the index of this value
     */
    virtual double index(double v) const;

    //@override
    bool contains(const QString& name, bool inclusive = true) const ;

    //@override
    bool isValid() const;

    //@override
    void add(DomainItem *item) ;

    //@override
    void add(SPDomainItem item);

    //@override
    void remove(const QString& nm) ;

    /**
     * generates a String representation of this NumericItemRange
     *
     * @return a string
     */
    QString toString() const;
    void clear();

    //@override
    ItemRange *clone() const;

    /**
      ??
     * override of operator<<
     * this operator will now try to add the string behind it to this NumericItemRange,
     * it will fail at invalid strings
     * valid string:
     * max length = 3, different numbers should be seperated by ' '
     * min length = 1
     * example: this << "'1''2''3'";
     *
     * @param itemdef the new item to be added
     * @return this
     */
    NumericItemRange &operator <<(const QString &itemdef);

    //@override
    qint32 gotoIndex(qint32 index, qint32 step) const;

protected:
    void addRange(const ItemRange &range);
private:
    std::vector<SPNumericItem> _items;
    bool validNumber(QString value) const;
    bool alignWithParent(const IDomain& dom);
};

typedef QSharedPointer<NumericItemRange> SPNumericItemRange;
}

#endif // NUMERICITEMRANGE_H
