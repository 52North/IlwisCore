#ifndef VALUEDOMAINDATA_H
#define VALUEDOMAINDATA_H

#include "Kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT NumericDomain : public Domain
{
public:
    NumericDomain();
    NumericDomain(const Resource& resource);


    void setRange(Range *vr);
    QString value(const QVariant&) const;
    Domain::Containement contains(const QVariant &value) const;
    bool isCompatibleWith(const IDomain &dom) const;

    IlwisTypes valueType() const;
    IlwisTypes ilwisType() const;

    void setParent(const IDomain &dm);

protected:
    SPRange getRange() const;
private:
    long significantDigits(double m1);

    SPNumericRange _range;

};

typedef IlwisData<NumericDomain> INumericDomain;
}

#endif // VALUEDOMAINDATA_H
