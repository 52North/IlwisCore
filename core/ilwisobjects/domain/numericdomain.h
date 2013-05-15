#ifndef VALUEDOMAINDATA_H
#define VALUEDOMAINDATA_H

#include "Kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT NumericDomain : public Domain
{
public:
    NumericDomain();
    NumericDomain(const Resource& res);

    template<typename T> T *range() const{
        return dynamic_cast<T *>(_range.data());
    }
    void setRange(Range *vr);
    QString value(double v) const;

    IlwisTypes valueType() const;
    IlwisTypes ilwisType() const;
private:
    long significantDigits(double m1);

    PRange _range;

};

typedef IlwisData<NumericDomain> INumericDomain;
}

#endif // VALUEDOMAINDATA_H
