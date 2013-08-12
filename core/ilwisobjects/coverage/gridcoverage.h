#ifndef RASTERLAYER_H
#define RASTERLAYER_H

#include "Kernel_global.h"

namespace Ilwis {

class Resource;
class Grid;

class KERNELSHARED_EXPORT GridCoverage : public Coverage
{
public:

    friend class PixelIterator;
    friend class GridBlock;
    friend class Grid;
    friend class GridInterpolator;

    GridCoverage();
    GridCoverage(const Resource& res);
    ~GridCoverage();

    IlwisTypes ilwisType() const;
    virtual GridCoverage *copy() ;

    const Ilwis::IGeoReference &georeference() const;
    void georeference(const IGeoReference& grf) ;
    Size size() const;
    void size(const Size& sz);

    void copyBinary(const IlwisData<GridCoverage> &gc, int index);

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

    Resource resource(int mode=cmINPUT) const;
protected:
    Grid *grid();
    void copyTo(IlwisObject *obj);
    QScopedPointer<Grid> _grid;
    std::mutex _mutex;

private:
    IGeoReference _georef;
};

typedef IlwisData<GridCoverage> IGridCoverage;
}

Q_DECLARE_METATYPE(Ilwis::IGridCoverage)


#endif // RASTERLAYER_H
