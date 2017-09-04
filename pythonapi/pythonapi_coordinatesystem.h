#ifndef PYTHONAPI_COORDINATESYSTEM_H
#define PYTHONAPI_COORDINATESYSTEM_H

#include "pythonapi_ilwisobject.h"
#include "pythonapi_util.h"

namespace Ilwis{
    class CoordinateSystem;
    typedef IlwisData<CoordinateSystem> ICoordinateSystem;
}

namespace pythonapi {
    class CoordinateSystem : public pythonapi::IlwisObject{
        friend class Coverage;
        friend class Engine;
        friend class Geometry;
        friend class FeatureCoverage;
        friend class GeoReference;
        friend class Catalog;
        friend class RasterCoverage;
    private:
        CoordinateSystem(const Ilwis::ICoordinateSystem& cs);
    protected:
        virtual const QString getStoreFormat() const;
    public:
        CoordinateSystem(const std::string &resource);
        pythonapi::Envelope envelope();
        void setEnvelope(const pythonapi::Envelope& env);
        bool operator==(const CoordinateSystem& csy);
        bool operator!=(const CoordinateSystem& csy);

        std::string toWKT() const;
        std::string toProj4() const;
        pythonapi::Envelope convertEnvelope(const CoordinateSystem& pyCsy, const pythonapi::Envelope& pyEnv);

        pythonapi::Coordinate coord2coord(const CoordinateSystem& pyCsy, const pythonapi::Coordinate& pyCoord);

        static CoordinateSystem* toCoordinateSystem(Object *obj);
    };

} // namespace pythonapi

#endif // PYTHONAPI_COORDINATESYSTEM_H
