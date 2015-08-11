#ifndef REPRESENTATION_H
#define REPRESENTATION_H

#include "colorlookup.h"
#include "shapelookup.h"

namespace Ilwis {

class ShapeLookup;
typedef std::unique_ptr<ColorLookUp> UPColorLookUp;
typedef std::unique_ptr<ShapeLookUp> UPShapeLookUp;

class Domain;
typedef IlwisData<Domain> IDomain;

class KERNELSHARED_EXPORT Representation : public IlwisObject
{
public:
    Representation();
    Representation(const Resource& resource);

    IlwisData<Representation> copyWith(const IDomain& dom) const;
    const UPColorLookUp& colors() const;
    void colors(ColorLookUp *lookup);
    void shapes(ShapeLookUp* lookup);
    const UPShapeLookUp& shapes() const;

    IDomain domain() const;
    void domain(const IDomain& domain);

    bool isCompatible(const IDomain& otherDomain);
    IlwisTypes ilwisType() const;

    static IlwisData<Representation> defaultRepresentation(const Ilwis::IDomain &dom);

private:
    UPColorLookUp _colors;
    UPShapeLookUp _shapes;
    IDomain _domain;
};

typedef IlwisData<Representation> IRepresentation;
}

Q_DECLARE_METATYPE(Ilwis::IRepresentation)

#endif // REPRESENTATION_H
