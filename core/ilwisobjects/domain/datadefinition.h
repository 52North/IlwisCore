#ifndef DATADEFINITION_H
#define DATADEFINITION_H

namespace Ilwis {
class KERNELSHARED_EXPORT DataDefinition
{
public:
    DataDefinition(const DataDefinition &def);
    DataDefinition();
    ~DataDefinition();
    QSharedPointer<Range> range() const;
    /*!
     * \brief setRange
     * \param vr
     */
    void range(Ilwis::Range *vr);
    IDomain domain() const;
    virtual void domain(const IDomain& dom);

protected:
    IDomain _domain;
    PRange _range;
    PRange _stretchRange;
};
}

#endif // DATADEFINITION_H
