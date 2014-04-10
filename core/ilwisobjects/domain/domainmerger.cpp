#include <QUrl>
#include "kernel.h"
#include "connectorinterface.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "itemrange.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "itemdomain.h"
#include "thematicitem.h"
#include "domainmerger.h"

using namespace Ilwis;

DomainMerger::DomainMerger()
{
}

DomainMerger::DomainMerger(const IDomain &dom1, const IDomain &dom2) : _domain1(dom1), _domain2(dom2)
{

}

IDomain DomainMerger::mergeDomains(const IDomain &dom1, const IDomain &dom2)
{
    if ( _domain1.isValid() && _domain2.isValid() &&
         _domain1->valueType() == _domain2->valueType()){
        if ( dom1->valueType() == itNUMBER ){
            NumericDomainMerger merger(dom1, dom2);
            merger.merge();
        } else if ( dom1->valueType() == itINDEXEDITEM){
            ItemDomainMergerIndexedItems merger(dom1, dom2);
            merger.merge();

        } else if ( dom1->valueType() == itNAMEDITEM){
            ItemDomainMergerNamedItems merger(dom1, dom2);
            merger.merge();

        }
    }
    return _mergedDomain;
}

std::map<quint32, quint32> DomainMerger::renumberer() const
{
    return _renumber;
}

IDomain DomainMerger::mergedDomain() const
{
    return _mergedDomain;
}


//--------------------------------------------------------------------
NumericDomainMerger::NumericDomainMerger(const IDomain &dom1, const IDomain &dom2) : DomainMerger(dom1, dom2)
{

}

bool NumericDomainMerger::merge()
{
    INumericDomain numericDom;

    Range *newRange = NumericRange::merge(_domain1->range<NumericRange>(), _domain2->range<NumericRange>());
    if (!newRange)
        return false;
    numericDom.prepare();
    numericDom->range(newRange);
    _mergedDomain = numericDom;

    return true;
}

//--------------------------------------------------------------------
ItemDomainMergerIndexedItems::ItemDomainMergerIndexedItems(const IDomain &dom1, const IDomain &dom2) : DomainMerger(dom1, dom2)
{

}

bool ItemDomainMergerIndexedItems::merge()
{
    IIndexedIdDomain iddom;

    QSharedPointer<IndexedIdentifierRange> range1 = _domain1->range<IndexedIdentifierRange>();
    QSharedPointer<IndexedIdentifierRange> range2 = _domain1->range<IndexedIdentifierRange>();

    std::unique_ptr<IndexedIdentifierRange> newRange(IndexedIdentifierRange::merge(range1, range2));
    if ( newRange->count() == 0)
        return false;

    for(quint32 i=0; i < range2->count(); ++i) {
        _renumber[i] =  range1->count() + i + 1;
    }

    iddom.prepare();
    iddom->setRange(*newRange.get());
    _mergedDomain = iddom;
    return true;
}

//--------------------------------------------------------------------
ItemDomainMergerNamedItems::ItemDomainMergerNamedItems(const IDomain &dom1, const IDomain &dom2) : DomainMerger(dom1, dom2)
{

}

bool ItemDomainMergerNamedItems::merge()
{
    INamedIdDomain iddom;

    bool notPossible = (_domain1->parent() != _domain2->parent()) &&
            _domain1->parent().isValid() && _domain2->parent().isValid();

    if (!notPossible)
        return false;

    QSharedPointer<NamedIdentifierRange> range1 = _domain1->range<NamedIdentifierRange>();
    QSharedPointer<NamedIdentifierRange> range2 = _domain1->range<NamedIdentifierRange>();

    std::unique_ptr<NamedIdentifierRange> newRange(NamedIdentifierRange::merge(range1, range2));
    if ( newRange->count() == 0)
        return false;

    bool skipRenumber = (_domain1->parent() == _domain2->parent()) &&
            _domain1->parent().isValid() && _domain2->parent().isValid();

    if (!skipRenumber) {
        if (_domain1->parent().isValid() && !_domain2->parent().isValid()){
            renumber(newRange, range1, range2);
        } else if (_domain2->parent().isValid() && !_domain1->parent().isValid()){
            renumber(newRange, range2, range1);
        } else {
            renumber(newRange, range1, range2);
        }

        iddom.prepare();
        iddom->setRange(*(newRange.get()));
        _mergedDomain = iddom;
        return true;
    }
    return true;
}

void ItemDomainMergerNamedItems::renumber(const std::unique_ptr<NamedIdentifierRange>& newRange,
                                          const QSharedPointer<NamedIdentifierRange>& range1,
                                          QSharedPointer<NamedIdentifierRange>& range2 ) {
    quint32 maxraw = 0;
    for(int i=0; i < range1->count(); ++i) {
        maxraw = std::max(range1->itemByOrder(i)->raw(), maxraw);
    }
    for(int i=0; i < newRange->count(); ++i)    {
        SPDomainItem item = newRange->item(i);
        bool inrange1 = range1->contains(item->name());
        bool inrange2 = range2->contains(item->name());
        if ( !inrange1 && inrange2){
            _renumber[item->raw()] = ++maxraw;
            item->raw(maxraw);
        }
    }
}


