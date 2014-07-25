#ifndef CSYTRANSFORM_H
#define CSYTRANSFORM_H

namespace Ilwis {
class KERNELSHARED_EXPORT CsyTransform : public geos::geom::CoordinateFilter
{
public:
    CsyTransform(const ICoordinateSystem& source, const ICoordinateSystem& target);
    CsyTransform(CoordinateSystem *source,const ICoordinateSystem& target);

    void filter_rw(geos::geom::Coordinate *crd) const;
private:
    ICoordinateSystem _source;
    ICoordinateSystem _target;
};
}

#endif // CSYTRANSFORM_H
