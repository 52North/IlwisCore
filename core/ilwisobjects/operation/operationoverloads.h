#ifndef OPERATIONOVERLOADS_H
#define OPERATIONOVERLOADS_H


KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator+(const Ilwis::IRasterCoverage& raster1, const Ilwis::IRasterCoverage& raster2);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator*(const Ilwis::IRasterCoverage& raster1, const Ilwis::IRasterCoverage& raster2);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator/(const Ilwis::IRasterCoverage& raster1, const Ilwis::IRasterCoverage& raster2);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator+(const Ilwis::IRasterCoverage& raster1, double);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator-(const Ilwis::IRasterCoverage& raster1, double);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator*(const Ilwis::IRasterCoverage& raster1, double);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator/(const Ilwis::IRasterCoverage& raster1, double);

#endif // OPERATIONOVERLOADS_H
