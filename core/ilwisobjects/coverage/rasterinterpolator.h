#ifndef GRIDINTERPOLATOR_H
#define GRIDINTERPOLATOR_H

namespace Ilwis {
class Grid;

class KERNELSHARED_EXPORT RasterInterpolator
{
public:
    enum InterpolationMethod{ipNEARESTNEIGHBOUR, ipBILINEAR, ipBICUBIC};

    RasterInterpolator(const IRasterCoverage& raster, int method) ;
    double pix2value(const Pixeld &pix);
    double coord2value(const Coordinate& crd, quint32 z);

private:
    double bilinear(const Pixeld &pix) ;
    double bicubic(const Pixeld &pix) ;
    double bicubicPolynom(double values[], const double &delta);
    double bicubicResult(long row, long column, long z, const double &deltaCol);
    bool resolveRealUndefs(double values[]);
    long _nbrows[4], _nbcols[4];
    double _weight[4];
    double _yvalues[4], _xvalues[4];
    IRasterCoverage _gcoverage;
    const UPGrid &_grid; // for peformance reason we store this; will be valid aslong as the coverage is there
    IGeoReference _grf;
    int _method;
    bool _valid;
};
}

#endif // GRIDINTERPOLATOR_H
