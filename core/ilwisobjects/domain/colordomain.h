#ifndef COLORDOMAIN_H
#define COLORDOMAIN_H

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
    bool isCompatibleWith(const IDomain& dom) const;
    void range(Range *colorrange);

protected:
    QSharedPointer<Range> getRange() const;

private:

    QSharedPointer<ColorRange> _range;
};
}

#endif // COLORDOMAIN_H
