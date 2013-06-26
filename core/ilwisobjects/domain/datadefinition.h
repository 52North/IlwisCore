#ifndef DATADEFINITION_H
#define DATADEFINITION_H

namespace Ilwis {
class KERNELSHARED_EXPORT DataDefinition
{
public:
    enum DomainAxis{ daCOVERAGE, daINDEX};
    DataDefinition(const DataDefinition &def);
    DataDefinition(const IDomain& dm, Ilwis::Range *rng=0);
    DataDefinition();
    ~DataDefinition();
    DataDefinition& operator=(const DataDefinition& def1);
    QSharedPointer<Range> range(DomainAxis da=daCOVERAGE) const;
    void range(Ilwis::Range *vr,DomainAxis da=daCOVERAGE);
    IDomain domain(DomainAxis da=daCOVERAGE) const;
    void domain(const IDomain& dom,DomainAxis da=daCOVERAGE);
    bool isValid() const;

    static DataDefinition merge(const DataDefinition &def1, const DataDefinition &def2);

protected:
    IDomain _coveragedomain;
    IDomain _indexdomain;
    SPRange _coveragerange;
    SPRange _indexrange;
    SPRange _stretchRange;
};

}

#endif // DATADEFINITION_H
