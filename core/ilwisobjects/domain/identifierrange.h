#ifndef IDENTIFIERRANGE_H
#define IDENTIFIERRANGE_H

#include "kernel_global.h"
#include <QVector>
#include <unordered_map>
#include "itemrange.h"

namespace Ilwis {

class NamedIdentifier;
/**
 * Because this class had to fit an interface there are a couple of empty functions and parameters, these should not be used.
 *
 *
 */
class KERNELSHARED_EXPORT IndexedIdentifierRange : public ItemRange
{
public:

    /**
     * Constructor for an empty IndexedIdentifierRange.
     */
    IndexedIdentifierRange();

    /**
     * Constructs a new IndexedIdentifierRange and adds a IndexedIdentifier based on the given values.
     *
     * @param prefix the prefix that should be used for the IndexedIdentifier
     * @param count the count that should be used for the IndexedIdentifier
     * @param start empty parameter
     */
    IndexedIdentifierRange(const QString& prefix, quint32 count, quint32 start=0);

    /**
     * Checks if a certain string is contained in this IndexedIdentifierRange.
     *
     * @param name name of the object that is to be checked
     * @param inclusive empty parameter
     * @return true if the item is contained in this IndexedIdentifierRange
     */
    bool contains(const QVariant& name, bool inclusive = true) const;

    /**
     * Checks if this IndexedIdentifierRange is valid.
     *
     * @return true when valid
     */
    bool isValid() const;

    /**
     * Checks if this IndexedIdentifierRange is equal to another IndexedIdentifierRange.
     *
     * @param range the other range
     * @return true when equal
     */
    bool operator==(const IndexedIdentifierRange& range);

    /**
     * Removes an IndexedIdentifier from this range.
     *
     * @param nm name of the object to be removed
     */
    void remove(const QString& nm);

    /**
     * Copies this IndexedIdentifierRange range
     *
     * @return a copy of this
     */
    Range *clone() const;

    /**
     * Returns the IndexedIdentifier at the given index, gives an empty object when the index is larger than the position of the last object.
     *
     * @param index the index
     * @return IndexedIdentifier, can be empty
     */
    SPDomainItem item(quint32 index) const;

    /**
     * empty function
     *
     */
    SPDomainItem item(const QString &nam) const;

    /**
     * returns the item at the given index
     * @param the index
     * @return the item at the index
     */
    SPDomainItem itemByOrder(quint32 index) const;

    /**
     * Adds a new IndexedIdentifier to this range, only works when item is valid
     *
     * @param item the item that has to be added
     */
    void add(DomainItem *item);

    /**
     * Adds a new IndexedIdentifier to this range, only works when item is valid
     *
     * @param item the item that has to be added
     */
    void add(SPDomainItem item);
    void add(const QVariant& v);

    /**
     * Creates a QString with the value from a QVariant, only works when the QVariant is valid, will return iUNDEF when this is not the case.
     *
     * @param v the QVariant to be converted
     * @return the value in string form
     */
    QVariant impliedValue(const QVariant& v) const;

    /**
     * Creates and returns a String representation of this IndexedIdentifierRange
     *
     * @return a String of this object
     */
    QString toString() const;

    /**
     * non existant in cpp file
     */
    quint32 raw(const QString &item) const;

    /**
     * Returns the current count.
     *
     * @return the current count
     */
    quint32 count() const;

    /**
     * Changes the current count to a new value.
     *
     * @param nr the new count
     */
    void count(quint32 nr);

    /**
     * Checks if this object is continuous, which it is not.
     *
     * @return false
     */
    virtual bool isContinuous() const;

    /**
     * empty function
     */
    void interpolation(const QString&) {}

    /**
     * calculates the next index based on the current index and the size of the step
     * cannot go below 0 or over the count limit
     *
     * @param index the current index
     * @param step the size of the step, can be negative
     * @return the new index( 0=>result <=count)
     */
    qint32 gotoIndex(qint32 index, qint32 step) const;
    IlwisTypes valueType() const;
    void clear();
    void store(QDataStream& stream);
    void load(QDataStream& stream);

    /**
     * merges 2 IndexedIdentifierRange 's into one
     *
     * @param nr1 one of the IndexedIdentifierRanges to be merged
     * @param nr2 one of the IndexedIdentifierRanges to be merged
     * @param rnm empty parameter
     * @return the result of the merging
     */
    static IndexedIdentifierRange *merge(const QSharedPointer<IndexedIdentifierRange>& nr1, const QSharedPointer<IndexedIdentifierRange>& nr2,RenumberMap *rnm=0);
    static SPDomainItem valueAt(quint32& index, const Ilwis::Range *rng);
    static QString valueAsString(quint32 &index, const Range *rng);

private:
   bool alignWithParent(const IDomain& dom);
   SPIndexedIdentifier _start;
   std::map<quint32, SPDomainItem> _existingItems;
   quint32 _count;
};

class KERNELSHARED_EXPORT NamedIdentifierRange : public ItemRange
{
public:
    /**
     * Constructs an empty NamedIdentifierRange
     */
    NamedIdentifierRange();

    ~NamedIdentifierRange();

    /**
     * Checks if this NamedIdentifierRange contains a certain NamedIdentifier
     *
     * @param name the name of the identifier
     * @param inclusive empty parameter
     * @return true when the identifier is in the range
     */
    bool contains(const QVariant& name, bool inclusive = true) const;

    /**
     * Checks if this NamedIdentifierRange is valid
     * @return true when valid
     */
    bool isValid() const;

    /**
     * Compares another NamedIdentifierRange with this one
     *
     * @param range another NamedIdentifierRange
     * @return true when they are the same (equal)
     */
    bool operator==(const ItemRange& range) const;

    /**
     * Adds a new NamedIdentifier to this NamedIdentifierRange, will fail if the item you want to add is invalid
     *
     * @param item the new NamedIdentifier that should be added
     */
    void add(DomainItem *item);

    /**
     * Adds a new NamedIdentifier to this NamedIdentifierRange, will fail if the item you want to add is invalid
     *
     * @param item the new NamedIdentifier that should be added
     */
    void add(SPDomainItem item);
    void add(const QVariant& v);

    /**
     * Removes an NamedIdentifier from this NamedIdentifierRange
     *
     * @param item the name of the NamedIdentifier that should be removed
     */
    void remove(const QString& item);

    /**
     * Returns the value at a certain Index, will fail if the supplied QVariant is invalid
     *
     * @param v the QVariant with the index
     * @return the value at the index or sUNDEF
     */
    QVariant impliedValue(const QVariant& v) const;

    /**
     * override of the << operator, the << operator now adds the string as a NamedIdentifier into this range
     *
     * @param itemdef The name of the NamedIdentifier that should be added
     * @return this with an extra identifier
     */
    NamedIdentifierRange& operator<<(const QString& itemdef);

    /**
     * Tries to find a NamedIdentifier in this range with the same raw value as specified. if it fails it will return an empty domain item
     * @param iraw the raw searched for
     * @return the requested NamedIdentifier or an empty SPDomainItem
     */
    SPDomainItem item(quint32 iraw) const;

    /**
     * Tries to find a NamedIdentifier in this range with the same name as specified, if it fail it will return an empty domain item
     * @param nam the name seached for
     * @return the requested NamedIdentifier or an empty SPDomainItem
     */
    SPDomainItem item(const QString &nam) const;

    /**
     * Query for the NamedIdentifier at the given index
     * @param index the index
     * @return the NamedIdentifier at the index, or an empty SPDomainItem if the index is invalid
     */
    SPDomainItem itemByOrder(quint32 index) const;

    /**
     * Creates a clone of this range
     *
     * @return a clone of this range
     */
    Range *clone() const;

    /**
     * Creates a String representation of this object
     * @return a string
     */
    QString toString() const;

    /**
     * Checks if this NamedIdentifierRange is continuous (its not)
     * @return false
     */
    virtual bool isContinuous() const;

    /**
     * empty function
     */
    void interpolation(const QString&) {}
    void store(QDataStream& stream);
    void load(QDataStream& stream);

    /**
     * calculates the next index based on the current index and the size of the step
     * cannot go below 0 or over the count limit
     *
     * @param index the current index
     * @param step the size of the step, can be negative
     * @return the new index( 0=>result <=count)
     */
    qint32 gotoIndex(qint32 index, qint32 step) const;

    /**
     * Returns the current count.
     *
     * @return the current count
     */
    quint32 count() const;
    IlwisTypes valueType() const;
    void clear();

    /**
     * merges 2 NamedIdentifierRange 's into one
     *
     * @param nr1 one of the IndexedIdentifierRanges to be merged
     * @param nr2 one of the IndexedIdentifierRanges to be merged
     * @param rnm specification of how to renumber the new range default=0
     * @return the result of the merging
     */
    static NamedIdentifierRange *merge(const QSharedPointer<NamedIdentifierRange>& nr1, const QSharedPointer<NamedIdentifierRange>& nr2,RenumberMap *rnm=0);

    static QString valueAsString(quint32 &index, const Range *rng);
    static SPDomainItem valueAt(quint32 &index, const Range *rng);
protected:
    template<typename T> static void addItems(ItemRange *items,
                                         const QSharedPointer<NamedIdentifierRange>& nr1,
                                         const QSharedPointer<NamedIdentifierRange>& nr2,
                                            RenumberMap *rnm=0){
        quint32 maxraw = 0;
        for(int i=0; i < nr1->count(); ++i) {
            maxraw = std::max(nr1->itemByOrder(i)->raw(), maxraw);
            items->add(nr1->itemByOrder(i)->clone());
        }
        for(int i=0; i < nr2->count(); ++i)    {
            SPDomainItem item = nr2->item(i);
            if (!items->contains(item->name())) {
                QSharedPointer<T> newid((T*)item->clone());
                if ( rnm)
                    newid->raw(maxraw);
                items->add(newid);
            }
            if ( rnm)
                (*rnm)[item->raw()] = maxraw;
            ++maxraw;
        }
    }

protected:
    std::map<QString, SPNamedIdentifier> _byName;
    QString asString() const;

private:
    bool alignWithParent(const IDomain& dom);
    std::vector<SPNamedIdentifier> _byRaw;
    std::vector<SPNamedIdentifier> _byOrder;
};

class KERNELSHARED_EXPORT ThematicRange : public NamedIdentifierRange {
public:
    ThematicRange();
    ~ThematicRange() {}
    ThematicRange& operator<<(const QString& itemdef);
    static ThematicRange *merge(const QSharedPointer<ThematicRange> &nr1, const QSharedPointer<ThematicRange> &nr2, Ilwis::RenumberMap *renumberer);
    Range *clone() const;
    static SPDomainItem valueAt(quint32 &index, const Range *rng);
    static QString valueAsString(quint32 &index, const Range *rng);
    IlwisTypes valueType() const;
    void store(QDataStream& stream);
    void load(QDataStream& stream);
    QString toString() const;
};

}

#endif // IDENTIFIERRANGE_H
