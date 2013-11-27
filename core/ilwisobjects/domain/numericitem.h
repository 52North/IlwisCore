#ifndef NUMERICITEM_H
#define NUMERICITEM_H

namespace Ilwis {

class  NumericItemRange;

class KERNELSHARED_EXPORT NumericItem : public DomainItem
{
public:
    friend class NumericItemRange;

    /**
     * Constructor for an empty NumericItem
     */
    NumericItem();

    /**
     * Constructor for a NumericItem with range vr
     * @param vr the NumbericRange of this item
     */
    NumericItem(const NumericRange &vr);

    virtual ~NumericItem() {}

    //@override
    bool isValid() const;

    //@override
    QString name() const ;

    //@override
    DomainItem *clone() const;

    //@override
    IlwisTypes valueType() const;

    /**
     * Query for the range of this NumbericItem
     * @return the NumbericRange of this
     */
    NumericRange range() const;

    /**
     * Sets a new NumbericRange on this NumbericItem
     * @param valuerange the new range
     */
    void range(const NumericRange &valuerange);

    /**
     * Creates an empty NumbericItemRange
     *
     * @return and empty NumbericItemRange
     */
    void clear();

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
