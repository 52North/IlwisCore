#ifndef GRIDINTERPOLATOR_H
#define GRIDINTERPOLATOR_H

namespace Ilwis {
class Grid;

class GridInterpolator
{
public:
    GridInterpolator(Grid& grid);
    double pix2value(const Point3D<double> &pix, int method);

private:
    double bilinear(const Point3D<double> &pix) ;
    double bicubic(const Point3D<double> &pix) ;
    double bicubicPolynom(double values[], const double &delta);
    double bicubicResult(long row, long column, const double &deltaCol);
    bool resolveRealUndefs(double values[]);
    Grid& _grid;
    long _nbrows[4], _nbcols[4];
    double _weight[4];
    double _yvalues[4], _xvalues[4];
};
}

#endif // GRIDINTERPOLATOR_H
