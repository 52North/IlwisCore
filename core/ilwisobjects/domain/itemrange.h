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
    virtual DomainItem *item(quint32 index) const = 0;
    virtual DomainItem *item(const QString &nam) const = 0;
    virtual bool contains(const QString& name) const = 0;
    virtual bool isValid() const = 0;
    virtual void add(DomainItem *item) = 0;
    virtual void remove(const QString& nm) = 0;


    static DomainItem *create(const QString& type);
    static void addCreateItem(const QString& type, CreateItemFunc func);

private:
    static QHash<QString, CreateItemFunc> _createItem;
};
}

#endif // ITEMRANGE_H
