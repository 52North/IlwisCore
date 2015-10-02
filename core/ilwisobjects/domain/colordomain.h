#ifndef COLORDOMAIN_H
#define COLORDOMAIN_H

#include "domain.h"
#include "colorrange.h"

namespace Ilwis {
class KERNELSHARED_EXPORT ColorDomain : public Domain
{
public:
    ColorDomain();
    ColorDomain( const Resource& resource);
    IlwisTypes valueType() const;
    IlwisTypes ilwisType() const;
    QVariant impliedValue(const QVariant& value) const    ;
    Containement contains(const QVariant& value) const;
    bool isCompatibleWith(const Ilwis::IlwisObject *dom, bool strict=false) const;
    void range(Range *colorrange);
    bool isOrdered() const;
    void setParent(const IDomain& dm);

    IlwisObject *clone();
protected:
    QSharedPointer<Range> getRange() const;

private:

    QSharedPointer<Range> _range;
    void copyTo(IlwisObject *obj);
};

typedef IlwisData<ColorDomain> IColorDomain;
}

#endif // COLORDOMAIN_H
