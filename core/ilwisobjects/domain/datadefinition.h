#ifndef DATADEFINITION_H
#define DATADEFINITION_H

namespace Ilwis {
class KERNELSHARED_EXPORT DataDefinition
{
public:
    DataDefinition(const DataDefinition &def);
    DataDefinition(const IDomain& dm, Ilwis::Range *rng=0);
    DataDefinition();
    ~DataDefinition();
    DataDefinition& operator=(const DataDefinition& def1);
    QSharedPointer<Range> range() const;
    void range(Ilwis::Range *vr);
    IDomain domain() const;
    void domain(const IDomain& dom);
    bool isValid() const;

    static DataDefinition merge(const DataDefinition &def1, const DataDefinition &def2);

protected:
    IDomain _domain;
    SPRange _range;
    SPRange _stretchRange;
};

KERNELSHARED_EXPORT bool operator==(const DataDefinition& def1, const DataDefinition& def2);
KERNELSHARED_EXPORT bool operator!=(const DataDefinition& def1, const DataDefinition& def2);

}

#endif // DATADEFINITION_H
