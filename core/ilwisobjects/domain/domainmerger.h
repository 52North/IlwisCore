#ifndef DOMAINMERGER_H
#define DOMAINMERGER_H

namespace Ilwis{
/**
 * The DomainMerger class merges domains, when possible
 */
class DomainMerger
{
public:

    /**
     * Constructor for an empty DomainMerger
     */
    DomainMerger();

    /**
     * Creates a DomainMerger from 2 IDomain 's
     * Does not yet do the actual merging
     *
     * @param dom1 an IDomain you want merged
     * @param dom2 an IDomain you want merged
     */
    DomainMerger(const IDomain& dom1, const IDomain& dom2);

    /**
     * Merges the domains and returns the result
     *
     * @param dom1 first domain to be merged
     * @param dom2 second domain to be merged
     * @return the result of the merge, can be null if the domains are not compatible
     */
    IDomain mergeDomains(const IDomain& dom1, const IDomain& dom2);

    /**
     * Query for the renumberer
     * @return the renumberer
     */
    std::map<quint32, quint32> renumberer() const;

    /**
     * merges the 2 domains, which have been specified in the constructor
     * requires the 2 domains to be compatible
     * @return true when the merging was succesfull
     */
    virtual bool merge() = 0;

    /**
     * Query for the merged domain in this DomainMerger
     * based on the 2 domains specified in the constructor
     * @return the result of the merging
     */
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
