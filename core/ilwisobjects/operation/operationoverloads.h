#ifndef OPERATIONOVERLOADS_H
#define OPERATIONOVERLOADS_H
#include "kernel_global.h"

KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator+(const Ilwis::IRasterCoverage& raster1, double);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator-(const Ilwis::IRasterCoverage& raster1, double);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator*(const Ilwis::IRasterCoverage& raster1, double);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator/(const Ilwis::IRasterCoverage& raster1, double);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator+(const Ilwis::IRasterCoverage& raster1, const Ilwis::IRasterCoverage& raster2);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator*(const Ilwis::IRasterCoverage& raster1, const Ilwis::IRasterCoverage& raster2);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator/(const Ilwis::IRasterCoverage& raster1, const Ilwis::IRasterCoverage& raster2);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator-(const Ilwis::IRasterCoverage& raster1, const Ilwis::IRasterCoverage& raster2);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator/(double number,const Ilwis::IRasterCoverage &raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator-(double number,const Ilwis::IRasterCoverage &raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator<(const Ilwis::IRasterCoverage& raster1, double);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage sin(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage cos(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage tan(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage asin(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage acos(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage atan(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage log10(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage sqrt(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage ln(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage ceil(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage floor(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage sign(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage cosh(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage sinh(const Ilwis::IRasterCoverage& raster1);


#endif // OPERATIONOVERLOADS_H
