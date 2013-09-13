#ifndef RASTERLAYER_H
#define RASTERLAYER_H

#include "Kernel_global.h"

namespace Ilwis {

class Resource;
class Grid;

class KERNELSHARED_EXPORT RasterCoverage : public Coverage
{
public:

    friend class PixelIterator;
    friend class GridBlock;
    friend class Grid;
    friend class GridInterpolator;

    RasterCoverage();
    RasterCoverage(const Resource& res);
    ~RasterCoverage();

    IlwisTypes ilwisType() const;
    virtual RasterCoverage *copy() ;

    const Ilwis::IGeoReference &georeference() const;
    void georeference(const IGeoReference& grf) ;
    Size size() const;
    void size(const Size& sz);

    void copyBinary(const IlwisData<RasterCoverage> &gc, int index);

    double coord2value(const Coordinate &c){
        if ( _georef->isValid() && c.isValid()) {
            Point2D<double> pix = _georef->coord2Pixel(c);
            return pix2value(pix);
        }
        return rUNDEF;
    }

    double pix2value(const Point3D<double>& pix){
        if ( _georef->isValid() && !connector().isNull()) {
            if ( _grid.isNull()) {
                _grid.reset(connector()->loadGridData(this));
                if (_grid.isNull())
                    return rUNDEF;
            }

            double v = _grid->value(pix);
            return datadef().range()->ensure(v);
        }
        return rUNDEF;
    }
    IlwisData<RasterCoverage> get(quint32 index1, quint32 index2 = iUNDEF);

    Resource resource(int mode=cmINPUT) const;
protected:
    Grid *grid();
    void copyTo(IlwisObject *obj);
    QScopedPointer<Grid> _grid;
    Size _size;
    std::mutex _mutex;

private:
    IGeoReference _georef;
};

typedef IlwisData<RasterCoverage> IRasterCoverage;
}

Q_DECLARE_METATYPE(Ilwis::IRasterCoverage)


#endif // RASTERLAYER_H
