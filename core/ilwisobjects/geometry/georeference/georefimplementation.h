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
    Size size() const;
    void size(const Size& sz);
    bool centerOfPixel() const;
    void centerOfPixel(bool yesno);
    bool compute();
protected:

    ICoordinateSystem _csy;
    QSize _size;
    bool _centerOfPixel;
};
}

#endif // GEOREFIMPLEMENTATION_H
