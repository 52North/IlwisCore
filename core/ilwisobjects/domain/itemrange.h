#ifndef ITEMRANGE_H
#define ITEMRANGE_H

namespace Ilwis {

class DomainItem;

typedef DomainItem* (*CreateItemFunc)(const QString&);

class ItemRange : public Range
{
public:
    ItemRange() {}

    virtual QString value(quint32 index) const = 0;
    virtual quint32 count() const = 0;
    virtual SPDomainItem item(quint32 index) const = 0;
    virtual SPDomainItem item(const QString &nam) const = 0;
    virtual bool contains(const QString& name) const = 0;
    virtual bool isValid() const = 0;
    virtual void add(DomainItem *item) = 0;
    virtual void remove(const QString& nm) = 0;
    double ensure(double v, bool inclusive = true) const;


    static DomainItem *create(const QString& type);
    static void addCreateItem(const QString& type, CreateItemFunc func);

    static ItemRange *merge(const QSharedPointer<ItemRange>& nr1, const QSharedPointer<ItemRange>& nr2);

private:
    static QHash<QString, CreateItemFunc> _createItem;
};

typedef QSharedPointer<ItemRange> SPItemRange;
}

#endif // ITEMRANGE_H
