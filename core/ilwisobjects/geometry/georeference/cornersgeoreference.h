#ifndef CORNERSGEOREFERENCE_H
#define CORNERSGEOREFERENCE_H

#include "kernel_global.h"

namespace Ilwis {

class GeoRefImplementation;

class KERNELSHARED_EXPORT CornersGeoReference : public SimpelGeoReference
{
public:
    CornersGeoReference();
    CornersGeoReference(const Resource& resource);
    void envelope(const Envelope& env);
    bool compute();
    QSize computeGridSize() const;
    bool isCornersOfCorners() const;
    Envelope envelope() const;
    GeoRefImplementation *clone();
    static QString typeName();


    static GeoRefImplementation *create();
private:
    void copyTo(GeoRefImplementation *impl);
    Envelope _envelope;
};

typedef IlwisData<CornersGeoReference> ICornersGeoReference;
}

#endif // CORNERSGEOREFERENCE_H
