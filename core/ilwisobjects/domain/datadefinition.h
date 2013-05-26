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
    QSharedPointer<Range> range() const;
    void range(Ilwis::Range *vr);
    IDomain domain() const;
    void domain(const IDomain& dom);
    bool isValid() const;

protected:
    IDomain _domain;
    PRange _range;
    PRange _stretchRange;
};

KERNELSHARED_EXPORT DataDefinition operator+(const DataDefinition& def1, const DataDefinition& def2);
}

#endif // DATADEFINITION_H
