#ifndef CORNERSGEOREFERENCE_H
#define CORNERSGEOREFERENCE_H

#include "Kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT CornersGeoReference : public SimpelGeoReference
{
public:
    CornersGeoReference();
    CornersGeoReference(const Resource& res);
    void setEnvelope(const Box2D<double>& env);
    bool compute();
    QSize computeGridSize() const;
    bool isCornersOfCorners() const;
    Box2D<double> envelope() const;
    static QString typeName();

    static GeoRefImplementation *create();
private:
    Box2D<double> _envelope;
    bool _isCornersOfCorners;

};

typedef IlwisData<CornersGeoReference> ICornersGeoReference;
}

#endif // CORNERSGEOREFERENCE_H
