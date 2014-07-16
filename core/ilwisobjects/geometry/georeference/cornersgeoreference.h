#ifndef CORNERSGEOREFERENCE_H
#define CORNERSGEOREFERENCE_H

#include "kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT CornersGeoReference : public SimpelGeoReference
{
public:
    CornersGeoReference();
    CornersGeoReference(const Resource& resource);
    void setEnvelope(const Envelope& env);
    bool compute();
    QSize computeGridSize() const;
    bool isCornersOfCorners() const;
    Envelope envelope() const;
    static QString typeName();

    static GeoRefImplementation *create();
private:
    Envelope _envelope;
};

typedef IlwisData<CornersGeoReference> ICornersGeoReference;
}

#endif // CORNERSGEOREFERENCE_H
