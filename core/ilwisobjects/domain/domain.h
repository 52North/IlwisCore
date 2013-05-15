#ifndef DOMAINDATA_H
#define DOMAINDATA_H

#include "Kernel_global.h"

namespace Ilwis {

class Domain;

typedef Ilwis::IlwisData<Ilwis::Domain> IDomain;

class KERNELSHARED_EXPORT Domain : public IlwisObject
{
public:
    Domain();
    Domain(const Resource& res);

    bool isStrict() const;
    void setStrict(bool yesno);
    virtual IlwisTypes valueType() const = 0;
    virtual QString value(double v) const = 0;
    IDomain parent() const;
    void setParent(const IDomain& dm);

    static IlwisTypes ilwType(const QVariant &v);

private:
    bool _strict;
    IDomain _parentDomain;
};
}

Q_DECLARE_METATYPE(Ilwis::IDomain)

#endif // DOMAINDATA_H
