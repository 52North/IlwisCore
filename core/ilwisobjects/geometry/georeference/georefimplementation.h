#ifndef GEOREFIMPLEMENTATION_H
#define GEOREFIMPLEMENTATION_H

namespace Ilwis {
class KERNELSHARED_EXPORT GeoRefImplementation : public GeoRefInterface, public Identity
{
public:
    GeoRefImplementation(const QString& type);
    virtual ~GeoRefImplementation();
    ICoordinateSystem coordinateSystem() const;
    void coordinateSystem(const ICoordinateSystem& csy);
    Size<> size() const;
    void size(const Size<>& sz);
    virtual Envelope envelope() const;
    virtual void envelope(const Envelope&);
    bool centerOfPixel() const;
    void centerOfPixel(bool yesno);
    bool compute();
    virtual GeoRefImplementation *clone() = 0;

    virtual bool isCompatible(const IGeoReference &georefOther) const;
protected:
    virtual void copyTo(GeoRefImplementation *impl);

    ICoordinateSystem _csy;
    Size<> _size;
    bool _centerOfPixel;
};
}

#endif // GEOREFIMPLEMENTATION_H
