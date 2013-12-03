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
     * Creates a DomainMerger from 2 IDomain 's<br>
     * Does not yet do the actual merging<br>
     * the domains may not be null
     *
     * @param dom1 an IDomain you want merged
     * @param dom2 an IDomain you want merged
     */
    DomainMerger(const IDomain& dom1, const IDomain& dom2);

    /**
     * Merges the domains and returns the result<br>
     * for a merge to work the domains must be valid and must have the same valuetypes<br>
     *
     * the merged domain will be saved in this domainmerger<br>
     * which means if you want to acces an already merged domain again you should use mergedDomain() instead of this
     *
     * @param dom1 first domain to be merged
     * @param dom2 second domain to be merged
     * @return the result of the merge, can be null if the domains are not compatible
     */
    IDomain mergeDomains(const IDomain& dom1, const IDomain& dom2);

    /**
     * Query for the renumberer<br>
     * the renumberer is the map that is used to renumber the elements in the 2 soon to be merged domains
     * @return the renumberer (a map<quint32,quint32>)
     */
    std::map<quint32, quint32> renumberer() const;

    /**
     * merges the 2 domains, which have been specified in the constructor<br>
     * requires the 2 domains to be compatible. the actual merging details are in the relevant subclasses
     *
     * @return true when the merging was succesfull
     */
    virtual bool merge() = 0;

    /**
     * Query for the merged domain in this DomainMerger<br>
     * based on the 2 domains specified in the constructor<br>
     * only works after the domains have been merged, if the domains have not yet been merged this will return null
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
