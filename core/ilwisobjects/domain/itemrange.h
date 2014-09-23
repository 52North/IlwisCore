#ifndef ITEMRANGE_H
#define ITEMRANGE_H

#include "rangeiterator.h"

namespace Ilwis {

class DomainItem;
class Domain;

typedef IlwisData<Domain> IDomain;

typedef DomainItem* (*CreateItemFunc)(const QString&);
typedef QSharedPointer<DomainItem> SPDomainItem;
/**
 * Template for the ItemRange subclasses. A Range decides what kind of values the items on this range can take. a Range is an essential component of Domains
 * because it determines the possible values within a domain
 */
class KERNELSHARED_EXPORT ItemRange : public Range
{
public:
    template<typename T> friend class ItemDomain;

    /**
     * Transforms a QVariant into the String representation of this value on the range.<br>
     * Will only work when the given value is in this range.
     *
     * @param v The QVariant you want translated
     * @return a string with the value
     */
    virtual QVariant impliedValue(const QVariant& v) const = 0;

    /**
     * Query for an item based on the raw value of that item.<br>
     * Will not work if there is no item with this raw value in the range.(returns null or undef)
     *
     * @param raw The raw value of the item
     * @return the item
     */
    virtual SPDomainItem item(quint32 raw) const = 0;

    /**
     * Query for an item based on the name of that item.<br>
     * Will not work if there is no item with this name in the range.(returns null or undef)
     *
     * @param nam the name of the item
     * @return the item
     */
    virtual SPDomainItem item(const QString &nam) const = 0;

    /**
     * Query for an item based on the index of that item
     * Will not work if there is no item on this index in the range.(returns null or undef)
     *
     * @param index the index of the item
     * @return the item
     */
    virtual SPDomainItem itemByOrder(quint32 index) const = 0;

    /**
     * Checks if the ItemRange contains a certain item.
     * Returns true if there is and item which name matches the given string.
     *
     * @param name name of the item
     * @param inclusive template parameter
     * @return true if this range contains the item
     */
    virtual bool contains(const QVariant& name, bool inclusive = true) const = 0;

    /**
     * Checks if the ItemRange contains a certain range of items.<br>
     * The range must be valid.
     *
     * @param rng the range to be checked
     * @param inclusive template parameter
     * @return true if rng is contained in this ItemRange
     */
    virtual bool contains(SPRange rng, bool inclusive=true) const;

    /**
     * Checks if the ItemRange contains a certain range of items.<br>
     * The range must be valid.
     *
     * @param itemrng the range to be checked
     * @param inclusive template parameter
     * @return true if rng is contained in this ItemRange
     */
    virtual bool contains(const ItemRange& itemrng, bool inclusive=true) const;

    /**
     * checks the validity of this range.
     *
     * @return true when valid
     */
    virtual bool isValid() const = 0;

    /**
     * Adds an item to this ItemRange.<br>
     * the added item must be valid.
     *
     * @param item the item to be added
     */
    virtual void add(DomainItem *item) = 0;

    /**
     * Adds an item to this ItemRange.<br>
     * The added item must be valid.
     *
     * @param item the item to be added
     */
    virtual void add(SPDomainItem item) = 0;

    /**
     * Removes an item from this range.<br>
     * If there is no item of this name in the range nothing will happen.
     *
     * @param nm the name of the item to be removed
     */
    virtual void remove(const QString& nm) = 0;

    /**
     * at this moment a empty method, will always return the same double as you give it
     */
    QVariant ensure(const QVariant& v, bool inclusive = true) const;

    /**
     * adds a certain ItemRange to this range.<br>
     * In the case of duplicate items, only 1 will be kept in the final result.<br>
     * The range that has to be added must be valid.
     *
     * @param range the range to be added
     */
    virtual void addRange(const ItemRange& range);

    /**
     * combines a index and a step size into a new position
     * the index cannot be lower as 0, neither can it be higher than the maximum index.
     * step size may be negative.
     * @param index the (current) index
     * @param step step size
     * @return the new index
     */
    virtual qint32 gotoIndex(qint32 index, qint32 step) const =0;

    /**
     * Checks if you can create a certain type of item with this ItemRange??
     */
    static DomainItem *create(const QString& type);

    /**
     *??
     */
    static void addCreateItem(const QString& type, CreateItemFunc func);

    /**
     * Merges this ItemRange with another.<br>
     * Both ranges must be valid, and both ranges must be itemranges for this to work.
     *
     * @param otherRange the other range to be merged
     * @return the result of the merging
     */
    ItemRange *merge(const QSharedPointer<Range> &otherRange) const;

    virtual bool isOrdered() const;
    virtual void clear() = 0;
    /**
     * Merges 2 itemranges into a new one
     * Both ranges must be valid, and both ranges must be itemranges for this to work.
     *
     * @param nr1 the first ItemRange to be merged
     * @param nr2 the second ItemRange to be merged
     * @return
     */
    static ItemRange *merge(const QSharedPointer<ItemRange>& nr1, const QSharedPointer<ItemRange>& nr2);

    static SPDomainItem valueAt(quint32& index, const Range *rng);
    static QString valueAsString(quint32 &index, const Range *rng);
protected:
private:
    virtual bool alignWithParent(const IDomain& dom) = 0;
    static QHash<QString, CreateItemFunc> _createItem;
};

typedef QSharedPointer<ItemRange> SPItemRange;
typedef Ilwis::RangeIterator<SPDomainItem, Ilwis::ItemRange> ItemRangeIterator;

inline ItemRangeIterator begin(const Ilwis::ItemRange& rng) {
    return ItemRangeIterator(&rng);
}

inline ItemRangeIterator end(const Ilwis::ItemRange& rng) {
    auto iter = ItemRangeIterator(&rng);
    iter += rng.count();
    return iter;
}

}

#endif // ITEMRANGE_H
