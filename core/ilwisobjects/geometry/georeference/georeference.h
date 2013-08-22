#ifndef GEOREFERENCE_H
#define GEOREFERENCE_H

#include "Kernel_global.h"
#include "georefadapter.h"

namespace Ilwis {

class GeoRefImplementation;

class GeoRefInterface {
public:
    virtual Coordinate pixel2Coord(const Pixel_d&) const = 0;
    virtual Pixel_d coord2Pixel(const Coordinate& crd) const = 0;
    virtual double pixelSize() const = 0;
    virtual bool compute() = 0;
    virtual void coordinateSystem(const ICoordinateSystem& csy)=0;
    virtual Size size() const=0;
    virtual void size(const Size& sz)=0;
    virtual bool centerOfPixel() const=0;
    virtual void centerOfPixel(bool yesno)=0;
};

class KERNELSHARED_EXPORT GeoReference : public IlwisObject, public GeoRefInterface
{
public:
    GeoReference();
    GeoReference(const Resource& res);
    ~GeoReference();

    Coordinate pixel2Coord(const Pixel_d&) const;
    Pixel_d coord2Pixel(const Coordinate& crd) const;
    double pixelSize() const;
    bool compute();

    virtual Box2D<double> pixel2Coord(const Box2D<qint32> &box ) const;
    virtual Box2D<qint32> coord2Pixel(const Box2D<double>& box) const;
    ICoordinateSystem coordinateSystem() const;
    void coordinateSystem(const ICoordinateSystem& csy);
    Size size() const;
    void size(const Size& sz);
    bool centerOfPixel() const;
    void centerOfPixel(bool yesno);
    void adapter(GeoRefAdapter* adapt);
    bool isValid() const;
    void impl(GeoRefImplementation *impl);
    template<typename GrfT> QSharedPointer<GrfT> impl(){
        QSharedPointer<GrfT> sptr = _georefImpl.dynamicCast<GrfT>();
        if ( sptr.isNull())
            throw ErrorObject(TR(ERR_COULD_NOT_CONVERT_2).arg("georef").arg(GrfT::typeName()));
        return sptr;
    }
    template<typename GrfT> bool grfType() const {
        return dynamic_cast<GrfT *>(_georefImpl.data()) != 0;
    }

    static GeoReference *create(const QString& type);

    IlwisTypes ilwisType() const;


protected:
    const QScopedPointer<GeoRefAdapter> &adapter() const;
private:
    QSharedPointer<GeoRefImplementation> _georefImpl;
    QScopedPointer<GeoRefAdapter> _adapter;

};

typedef IlwisData<GeoReference> IGeoReference;
}

Q_DECLARE_METATYPE(Ilwis::IGeoReference)

#endif // GEOREFERENCE_H
