#ifndef DATADEFINITION_H
#define DATADEFINITION_H

namespace Ilwis {
class KERNELSHARED_EXPORT DataDefinition
{
public:
    enum DomainAxis{ daLAYER, daLAYERINDEX};
    DataDefinition(const DataDefinition &def);
    DataDefinition(const IDomain& dm, Ilwis::Range *rng=0);
    DataDefinition();
    ~DataDefinition();
    DataDefinition& operator=(const DataDefinition& def1);
    QSharedPointer<Range> range(DomainAxis da=daLAYER) const;
    void range(Ilwis::Range *vr,DomainAxis da=daLAYER);
    IDomain domain(DomainAxis da=daLAYER) const;
    void domain(const IDomain& dom,DomainAxis da=daLAYER);
    bool isValid() const;

    static DataDefinition merge(const DataDefinition &def1, const DataDefinition &def2);

protected:
    IDomain _layerdomain;
    IDomain _indexdomain;
    SPRange _layerRange;
    SPRange _indexrange;
    SPRange _stretchRange;
};

}

#endif // DATADEFINITION_H
