#ifndef VALUEDEFINER_H
#define VALUEDEFINER_H

namespace Ilwis {
class KERNELSHARED_EXPORT ValueDefiner
{
public:
    ValueDefiner();
    QSharedPointer<Range> range() const;
    /*!
     * \brief setRange
     * \param vr
     */
    void setRange(Ilwis::Range *vr);
    IDomain domain() const;
    virtual void setDomain(const IDomain& dom);

protected:
    IDomain _domain;
    PRange _range;
    PRange _stretchRange;
};
}

#endif // VALUEDEFINER_H
