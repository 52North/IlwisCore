#ifndef DOMAINMERGER_H
#define DOMAINMERGER_H

namespace Ilwis{
class DomainMerger
{
public:
    DomainMerger();
    DomainMerger(const IDomain& dom1, const IDomain& dom2);
    IDomain mergeDomains(const IDomain& dom1, const IDomain& dom2);
    std::map<quint32, quint32> renumberer() const;
    virtual bool merge() = 0;
    IDomain mergedDomain() const;

protected:
    IDomain _domain1;
    IDomain _domain2;
    IDomain _mergedDomain;
    std::map<quint32, quint32> _renumber;

};

class NumericDomainMerger : private DomainMerger{
    friend class DomainMerger;

    NumericDomainMerger(const IDomain& dom1, const IDomain& dom2);
    bool merge();
};

class ItemDomainMergerIndexedItems : private DomainMerger {
    friend class DomainMerger;

    ItemDomainMergerIndexedItems(const IDomain& dom1, const IDomain& dom2);
    bool merge();

};

class ItemDomainMergerNamedItems : private DomainMerger {
    friend class DomainMerger;

    ItemDomainMergerNamedItems(const IDomain& dom1, const IDomain& dom2);
    bool merge();


private:
    void renumber(const std::unique_ptr<Ilwis::NamedIdentifierRange> &newRange, const QSharedPointer<NamedIdentifierRange> &range1, QSharedPointer<NamedIdentifierRange> &range2);
};
}

#endif // DOMAINMERGER_H
