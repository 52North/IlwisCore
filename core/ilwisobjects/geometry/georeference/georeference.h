#ifndef GEOREFERENCE_H
#define GEOREFERENCE_H

#include "kernel_global.h"
#include "georefadapter.h"

namespace Ilwis {

class GeoRefImplementation;
class CoordinateSystem;
typedef IlwisData<CoordinateSystem> ICoordinateSystem;
class GeoRefImplementation;

class GeoRefInterface {
public:
    virtual Coordinate pixel2Coord(const Pixeld&) const = 0;
    virtual Pixeld coord2Pixel(const Coordinate& crd) const = 0;
    virtual double pixelSize() const = 0;
    virtual bool compute() = 0;
    virtual void coordinateSystem(const ICoordinateSystem& csy)=0;
    virtual Size<> size() const=0;
    virtual void size(const Size<>& sz)=0;
    virtual bool centerOfPixel() const=0;
    virtual void centerOfPixel(bool yesno)=0;
};

class KERNELSHARED_EXPORT GeoReference : public IlwisObject, public GeoRefInterface
{
public:
    GeoReference();
    GeoReference(const Resource& resource);
    ~GeoReference();

    Coordinate pixel2Coord(const Pixeld& pixel) const;
    Pixeld coord2Pixel(const Coordinate& crd) const;
    double pixelSize() const;
    bool compute();

    virtual Envelope pixel2Coord(const BoundingBox &box ) const;
    virtual BoundingBox coord2Pixel(const Envelope &box) const;
    ICoordinateSystem coordinateSystem() const;
    void coordinateSystem(const ICoordinateSystem& csy);
    Size<> size() const;
    void size(const Size<>& sz);
    Envelope envelope() const;
    void envelope(const Envelope& env);
    bool centerOfPixel() const;
    void centerOfPixel(bool yesno);
    bool isCompatible(const IlwisData<GeoReference>& georefOther) const;
    bool canUse(const IlwisObject *obj, bool strict=false) const ;
    void adapter(GeoRefAdapter* adapt);
    bool isValid() const;
    void impl(GeoRefImplementation *impl);
    template<typename GrfT> QSharedPointer<GrfT> as(){
        QSharedPointer<GrfT> sptr = _georefImpl.dynamicCast<GrfT>();
        if ( sptr.isNull())
            throw ErrorObject(TR(ERR_COULD_NOT_CONVERT_2).arg("georef").arg(GrfT::typeName()));
        return sptr;
    }
    template<typename GrfT> bool grfType() const {
        return dynamic_cast<GrfT *>(_georefImpl.data()) != 0;
    }
    IlwisObject *clone();
    void copyTo(IlwisObject *obj);

    void create(const QString& type);

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
