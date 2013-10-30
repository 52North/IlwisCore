#ifndef IDENTIFIERRANGE_H
#define IDENTIFIERRANGE_H

#include "Kernel_global.h"
#include <QVector>

namespace Ilwis {

class NamedIdentifier;

class KERNELSHARED_EXPORT IndexedIdentifierRange : public ItemRange
{
public:

    /**
     * Constructor for an empty IndexedIdentifierRange
     */
    IndexedIdentifierRange();

    /**
     * Constructs a new IndexedIdentifierRange from the given values
     *
     * @param prefix the prefix that should be used
     * @param count
     * @param start
     */
    IndexedIdentifierRange(const QString& prefix, quint32 count, quint32 start=0);

    /**
     * @brief contains
     * @param name
     * @param inclusive
     * @return
     */
    bool contains(const QString& name, bool inclusive = true) const;

    /**
     * @brief isValid
     * @return
     */
    bool isValid() const;

    /**
     * @brief operator ==
     * @param range
     * @return
     */
    bool operator==(const IndexedIdentifierRange& range);

    /**
     * @brief remove
     * @param nm
     */
    void remove(const QString& nm);

    /**
     * @brief clone
     * @return
     */
    Range *clone() const;

    /**
     * @brief item
     * @param index
     * @return
     */
    SPDomainItem item(quint32 index) const;

    /**
     * @brief item
     * @param nam
     * @return
     */
    SPDomainItem item(const QString &nam) const;

    /**
     * @brief itemByOrder
     * @param index
     * @return
     */
    SPDomainItem itemByOrder(quint32 index) const;

    /**
     * @brief add
     * @param item
     */
    void add(DomainItem *item);

    /**
     * @brief add
     * @param item
     */
    void add(SPDomainItem item);

    /**
     * @brief value
     * @param v
     * @return
     */
    QString value(const QVariant& v) const;

    /**
     * @brief toString
     * @return
     */
    QString toString() const;

    /**
     * @brief raw
     * @param item
     * @return
     */
    quint32 raw(const QString &item) const;

    /**
     * @brief count
     * @return
     */
    quint32 count() const;

    /**
     * @brief count
     * @param nr
     */
    void count(quint32 nr);

    /**
     * @brief isContinuous
     * @return
     */
    virtual bool isContinuous() const;

    /**
     * @brief interpolation
     */
    void interpolation(const QString&) {}

    /**
     * @brief gotoIndex
     * @param index
     * @param step
     * @return
     */
    qint32 gotoIndex(qint32 index, qint32 step) const;

    /**
     * @brief merge
     * @param nr1
     * @param nr2
     * @param rnm
     * @return
     */
    static IndexedIdentifierRange *merge(const QSharedPointer<IndexedIdentifierRange>& nr1, const QSharedPointer<IndexedIdentifierRange>& nr2,RenumberMap *rnm=0);
private:
   bool alignWithParent(const IDomain& dom);
   SPIndexedIdentifier _start;
   quint32 _count;
};

class KERNELSHARED_EXPORT NamedIdentifierRange : public ItemRange
{
public:
    NamedIdentifierRange();
    ~NamedIdentifierRange();

    bool contains(const QString& name, bool inclusive = true) const;
    bool isValid() const;
    bool operator==(const ItemRange& range) const;

    void add(DomainItem *item);
    void add(SPDomainItem item);
    void remove(const QString& item);
    QString value(const QVariant& v) const;
    NamedIdentifierRange& operator<<(const QString& itemdef);
    SPDomainItem item(quint32 iraw) const;
    SPDomainItem item(const QString &nam) const;
    SPDomainItem itemByOrder(quint32 index) const;
    Range *clone() const;

    QString toString() const;
    virtual bool isContinuous() const;
    void interpolation(const QString&) {}
    qint32 gotoIndex(qint32 index, qint32 step) const;
    quint32 count() const;

    static NamedIdentifierRange *merge(const QSharedPointer<NamedIdentifierRange>& nr1, const QSharedPointer<NamedIdentifierRange>& nr2,RenumberMap *rnm=0);

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

private:
    bool alignWithParent(const IDomain& dom);
    std::map<QString, SPNamedIdentifier> _byName;
    std::map<quint32, SPNamedIdentifier> _byRaw;
    std::vector<SPNamedIdentifier> _byOrder;
};

class KERNELSHARED_EXPORT ThematicRange : public NamedIdentifierRange {
public:
    ThematicRange();
    ~ThematicRange() {}
    ThematicRange& operator<<(const QString& itemdef);
    static ThematicRange *merge(const QSharedPointer<ThematicRange> &nr1, const QSharedPointer<ThematicRange> &nr2, Ilwis::RenumberMap *renumberer);
};

}

#endif // IDENTIFIERRANGE_H
