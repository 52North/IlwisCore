#ifndef NUMERICITEM_H
#define NUMERICITEM_H

namespace Ilwis {

class  NumericItemRange;

class KERNELSHARED_EXPORT NumericItem : public DomainItem, public Identity
{
public:
    friend class NumericItemRange;

    /**
     * Constructor for an empty NumericItem
     */
    NumericItem();

    /**
     * Constructor for a NumericItem with range vr.<br>
     * vr must be valid. vr will be set as the range of this item.
     *
     * @param vr the NumericRange of this item
     */
    NumericItem(const QString label, const NumericRange &vr);

    virtual ~NumericItem() {}

    //@override
    bool isValid() const;

    //@override
    QString name() const;

    //@override
    DomainItem *clone() const;

    //@override
    IlwisTypes valueType() const;

    /**
     * Query for the range of this NumericItem.<br>
     * Returns the numeric range of this item, if the range is not set it will return null
     *
     * @return the NumericRange of this
     */
    NumericRange range() const;

    /**
     * Sets a new NumericRange on this NumericItem.<br>
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
    static Ilwis::NumericItemRange *createRange();


    //@override
    static IlwisTypes valueTypeS();

private:
    NumericRange _valuerange;
};

struct CompareNumericItem {
    bool operator()(const NumericItem& it1, const NumericItem& it2)     {
        return it1.range() < it2.range();
    }
};

typedef QSharedPointer<NumericItem> SPNumericItem;

}

#endif // NUMERICITEM_H
