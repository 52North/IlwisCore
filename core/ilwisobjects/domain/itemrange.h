#ifndef ITEMRANGE_H
#define ITEMRANGE_H

namespace Ilwis {

class DomainItem;

typedef DomainItem* (*CreateItemFunc)(const QString&);
typedef QSharedPointer<DomainItem> SPDomainItem;

class KERNELSHARED_EXPORT ItemRange : public Range
{
public:
    template<typename T> friend class ItemDomain;

    ItemRange() {}

    virtual QString value(quint32 index) const = 0;
    virtual quint32 count() const = 0;
    virtual SPDomainItem item(quint32 raw) const = 0;
    virtual SPDomainItem item(const QString &nam) const = 0;
    virtual SPDomainItem itemByOrder(quint32 index) const = 0;
    virtual bool contains(const QString& name, bool inclusive = true) const = 0;
    virtual bool isValid() const = 0;
    virtual void add(DomainItem *item) = 0;
    virtual void add(SPDomainItem item) = 0;
    virtual void remove(const QString& nm) = 0;
    double ensure(double v, bool inclusive = true) const;
    virtual void addRange(const ItemRange& range);
    virtual qint32 gotoIndex(qint32 index, qint32 step) const =0;

    static DomainItem *create(const QString& type);
    static void addCreateItem(const QString& type, CreateItemFunc func);
    static ItemRange *merge(const QSharedPointer<ItemRange>& nr1, const QSharedPointer<ItemRange>& nr2);

protected:
private:
    virtual bool alignWithParent(const IDomain& dom) = 0;
    static QHash<QString, CreateItemFunc> _createItem;
};

typedef QSharedPointer<ItemRange> SPItemRange;
}

#endif // ITEMRANGE_H
