#include <QString>
#include <typeinfo>
#include "kernel.h"
#include "range.h"
#include "domainitem.h"
#include "itemrange.h"

using namespace Ilwis;

QHash<QString, CreateItemFunc> ItemRange::_createItem;

double ItemRange::ensure(double v, bool ) const
{
    //TODO  more advanced impl needed? may cost performance
    return v;
}

DomainItem *ItemRange::create(const QString& type){
    auto iter = _createItem.find(type);
    if ( iter != _createItem.end())
        return iter.value()(type);
    return 0;
}

void ItemRange::addCreateItem(const QString& type, CreateItemFunc func){
    _createItem[type] = func;
}

ItemRange *ItemRange::merge(const QSharedPointer<ItemRange> &nr1, const QSharedPointer<ItemRange> &nr2)
{
    //TODO
    return 0;
}

void ItemRange::addRange(const ItemRange &range)
{
    for(quint32 i=0; i < range.count(); ++i) {
        add(range.item(i)->clone());
    }
}
