#ifndef CORNERSGEOREFERENCE_H
#define CORNERSGEOREFERENCE_H

#include "Kernel_global.h"

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
    bool _isCornersOfCorners;

};

typedef IlwisData<CornersGeoReference> ICornersGeoReference;
}

#endif // CORNERSGEOREFERENCE_H
