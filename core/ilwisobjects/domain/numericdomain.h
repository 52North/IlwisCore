#ifndef VALUEDOMAINDATA_H
#define VALUEDOMAINDATA_H

#include "Kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT NumericDomain : public Domain
{
public:
    NumericDomain();
    NumericDomain(const Resource& res);


    void setRange(Range *vr);
    QString value(double v) const;
    Domain::Containement contains(const QString& value) const;

    IlwisTypes valueType() const;
    IlwisTypes ilwisType() const;

protected:
    SPRange getRange() const;
private:
    long significantDigits(double m1);

    SPRange _range;

};

typedef IlwisData<NumericDomain> INumericDomain;
}

#endif // VALUEDOMAINDATA_H
