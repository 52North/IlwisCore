#ifndef REPRESENTATION_H
#define REPRESENTATION_H

namespace Ilwis {

class ColorLookup;
typedef std::unique_ptr<ColorLookUp> UPColorLookUp;

class Domain;
typedef IlwisData<Domain> IDomain;

class KERNELSHARED_EXPORT Representation : public IlwisObject
{
public:
    Representation();
    Representation(const Resource& resource);
    const UPColorLookUp& colors() const;
    void colors(ColorLookUp *lookup);

    IDomain domain() const;
    void domain(const IDomain& domain);

    bool isCompatible(const IDomain& otherDomain);
    IlwisTypes ilwisType() const;

    static IlwisData<Representation> defaultRepresentation(const Ilwis::IDomain &dom);

private:
    UPColorLookUp _colors;
    IDomain _domain;
};

typedef IlwisData<Representation> IRepresentation;
}

#endif // REPRESENTATION_H
