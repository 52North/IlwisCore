#ifndef ITEMRANGE_H
#define ITEMRANGE_H

namespace Ilwis {

class DomainItem;

typedef DomainItem* (*CreateItemFunc)(const QString&);
typedef QSharedPointer<DomainItem> SPDomainItem;
/**
 * Template for the ItemRange subclasses, for the description of the virtual functions see the subclass you want to use
 */
class KERNELSHARED_EXPORT ItemRange : public Range
{
public:
    template<typename T> friend class ItemDomain;

    /**
     * Translates the given QVariant into a value in QString form
     *
     * @param v The QVariant you want translated
     * @return a string with the value
     */
    virtual QString value(const QVariant& v) const = 0;

    /**
     * Query for the amount of items in this range
     *
     * @return the amount of items
     */
    virtual quint32 count() const = 0;

    /**
     * Query for an item based on the raw value of that item
     *
     * @param raw The raw value of the item
     * @return the item
     */
    virtual SPDomainItem item(quint32 raw) const = 0;

    /**
     * Query for an item based on the name of that item
     *
     * @param nam the name of the item
     * @return the item
     */
    virtual SPDomainItem item(const QString &nam) const = 0;

    /**
     * Query for an item based on the index of that item
     *
     * @param index the index of the item
     * @return the item
     */
    virtual SPDomainItem itemByOrder(quint32 index) const = 0;

    /**
     * Checks if the ItemRange contains a certain item
     *
     * @param name name of the item
     * @param inclusive empty parameter
     * @return true if this range contains the item
     */
    virtual bool contains(const QString& name, bool inclusive = true) const = 0;

    /**
     * Checks if the ItemRange contains a certain range of items
     *
     * @param rng the range to be checked
     * @param inclusive empty parameter
     * @return true if rng is contained in this ItemRange
     */
    virtual bool contains(SPRange rng, bool inclusive=true) const;

    /**
     * Checks if the ItemRange contains a certain range of items
     *
     * @param itemrng the range to be checked
     * @param inclusive empty parameter
     * @return true if rng is contained in this ItemRange
     */
    virtual bool contains(const ItemRange& itemrng, bool inclusive=true) const;

    /**
     * checks if this item is valid
     *
     * @return true when valid
     */
    virtual bool isValid() const = 0;

    /**
     * Adds an item to this ItemRange
     *
     * @param item the item to be added
     */
    virtual void add(DomainItem *item) = 0;

    /**
     * Adds an item to this ItemRange
     *
     * @param item the item to be added
     */
    virtual void add(SPDomainItem item) = 0;

    /**
     * Removes an item from this range
     *
     * @param nm the name of the item to be removed
     */
    virtual void remove(const QString& nm) = 0;

    /**
     * at this moment a empty method, will always return the same double as you give it
     */
    double ensure(double v, bool inclusive = true) const;

    /**
     * adds a certain ItemRange to this range
     *
     * @param range the range to be added
     */
    virtual void addRange(const ItemRange& range);

    /**
     * combines a index and a step size into a new position
     *
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
     * Merges this ItemRange with another
     *
     * @param otherRange the other range to be merged
     * @return the result of the merging
     */
    ItemRange *merge(const QSharedPointer<Range> &otherRange) const;

    /**
     * Merges 2 itemranges into a new one
     *
     * @param nr1 the first ItemRange to be merged
     * @param nr2 the second ItemRange to be merged
     * @return
     */
    static ItemRange *merge(const QSharedPointer<ItemRange>& nr1, const QSharedPointer<ItemRange>& nr2);

protected:
private:
    virtual bool alignWithParent(const IDomain& dom) = 0;
    static QHash<QString, CreateItemFunc> _createItem;
};

typedef QSharedPointer<ItemRange> SPItemRange;
}

#endif // ITEMRANGE_H
