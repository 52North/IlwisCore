#include "kernel.h"
#include "raster.h"
#include "rasterinterpolator.h"

using namespace Ilwis;

RasterInterpolator::RasterInterpolator(const IRasterCoverage& raster, int method) : _gcoverage(raster), _grid(_gcoverage->grid()),_method(method)  {

    _grf = _gcoverage->georeference();
    _valid = _grf.isValid() && _grid != 0;
}

double RasterInterpolator::pix2value(const Pixeld& pix) {
    double v = rUNDEF;
    switch( _method) {
    case 0: //nearestneighbour
        return _grid->value(pix);
    case 1: //bilinear
        return bilinear(pix);
    case 2: //bicubic
        return bicubic(pix);
    }
    return v;

}

double RasterInterpolator::coord2value(const Coordinate &crd, quint32 z)
{
     if (!_valid || !crd.isValid())
        return rUNDEF;
     Pixeld pix = _grf->coord2Pixel(crd);
     pix.z = z;
     return pix2value(pix);
}

double RasterInterpolator::bilinear(const Pixeld& pix) {
    double y = pix.y - 0.5;
    double x = pix.x - 0.5;

    _nbrows[1] = _nbrows[0] = (int)y;
    _nbrows[3] = _nbrows[2] = _nbrows[1] + 1;
    _nbcols[2] = _nbcols[0] = (int)x;
    _nbcols[3] = _nbcols[1] = _nbcols[0] + 1;
    double deltaY = y - _nbrows[0];
    double deltaX = x - _nbcols[0];
    _weight[0] = (1 - deltaY) * (1 - deltaX);
    _weight[1] = (1 - deltaY) * deltaX;
    _weight[2] = deltaY * (1 - deltaX);
    _weight[3] = deltaY * deltaX;
    double tot_weight=0.0, totValue=0.0;
    for (int i = 0; i < 4; ++i) {
        double rVal = _grid->value({_nbcols[i],_nbrows[i], pix.z});
        if (rVal != rUNDEF) {
            totValue +=  rVal * _weight[i];
            tot_weight += _weight[i];
        }
    }
    if (tot_weight < 0.1)
        return rUNDEF;
    else
        return totValue / tot_weight;

}

double RasterInterpolator::bicubic(const Pixeld &pix)
{
    double y = pix.y - 0.5;
    double x = pix.x - 0.5;
    long column = (long)x;
    long row = (long)y;
    double deltaY = y - row;
    double deltaX = x - column;
    for(short i=0; i<4; ++i)
        _yvalues[i]=bicubicResult(row-1+i, column, pix.z, deltaX);
    if(resolveRealUndefs(_yvalues))
      return bicubicPolynom(_yvalues, deltaY);


    return rUNDEF;
}

double RasterInterpolator::bicubicPolynom(double values[], const double& delta)
{
    double result = values[1] +
                     delta * (( values[2] - values[1]/2 - values[0]/3 -values[3]/6) +
                     delta * (( (values[2] + values[0])/2 - values[1]) +
                     delta * (( values[1] - values[2])/2 + (values[3] - values[0])/6)));
    return result;
}

double RasterInterpolator::bicubicResult(long row, long column, long z, const double& deltaCol)
{
  long i;
  if ( row >= _gcoverage->size().ysize())
       return rUNDEF;
  for( i=0; i<4; ++i){
      _xvalues[i]= _grid->value({column-1L+i,row, z});
  }
  if(resolveRealUndefs(_xvalues))
    return bicubicPolynom(_xvalues, deltaCol);

  return rUNDEF;
}

bool RasterInterpolator::resolveRealUndefs(double values[])
{
    if ( values[1]==rUNDEF){
        if (values[2]==rUNDEF)
          return false;
        else
          values[1]=values[2];
    }
    if ( values[2] == rUNDEF )
      values[2]=values[1];
    if ( values[0] == rUNDEF )
      values[0]=values[1];
    if ( values[3] == rUNDEF )
      values[3]=values[2];

    return true;
}

