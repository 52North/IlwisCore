#ifndef VALUEDOMAINDATA_H
#define VALUEDOMAINDATA_H

#include "Kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT ValueDomain : public Domain
{
public:
    ValueDomain();
    ValueDomain(const Resource& res);

    const Ilwis::PRange &range() const;
    void setRange(Range *vr);
    QString value(double v) const;

    virtual void debug();

    IlwisTypes valueType() const;
private:
    long significantDigits(double m1);

    PRange _range;

};

typedef IlwisData<ValueDomain> IValueDomain;
}

#endif // VALUEDOMAINDATA_H
