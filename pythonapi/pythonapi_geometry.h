#ifndef PYTHONAPI_GEOMETRY_H
#define PYTHONAPI_GEOMETRY_H

#include "pythonapi_object.h"
#include "pythonapi_coordinatesystem.h"

#include <memory>

namespace geos {
namespace geom {
    class Geometry;
}
}

namespace pythonapi {

    class Feature;
    class VertexIterator;

    class Geometry : public Object{
        friend class Feature;
        friend class FeatureCoverage;
        friend class RasterCoverage;
        friend class PixelIterator;
        friend class VertexIterator;
    public:
        Geometry(std::string wkt, const CoordinateSystem &csy);
        Geometry(Feature* feature);
        ~Geometry();

        bool __bool__() const ;
        std::string __str__();
        IlwisTypes ilwisType();
        void fromWKT(const std::string& wkt);
        geos::geom::Geometry* fromWKTReader(const std::string& wkt);
        std::string toWKT();
        CoordinateSystem coordinateSystem();
        void setCoordinateSystem(const CoordinateSystem& cs);
        Geometry* transform(const CoordinateSystem& cs);
        Envelope envelope();

        VertexIterator __iter__();

        bool isSimple() const;

        bool within(const Geometry& geometry) const;
        bool contains(const Geometry& geometry) const;
        bool disjoint(const Geometry& geometry) const;
        bool touches(const Geometry& geometry) const;
        bool intersects(const Geometry& geometry) const;
        bool crosses(const Geometry& geometry) const;
        bool overlaps(const Geometry& geometry) const;
        bool equals(const Geometry& geometry) const;
        bool equalsExact(const Geometry& geometry, double tolerance=0) const;
        bool covers(const Geometry& geometry) const;
        bool coveredBy(const Geometry& geometry) const;
        bool relate(const Geometry& geometry, const std::string& DE9IM_pattern) const;

        double distance(const Geometry& geometry) const;
        double getArea() const;
        double getLength() const;
        bool isWithinDistance(const Geometry& geometry, double cDistance) const;

        Geometry* buffer(double distance) const;
        Geometry* convexHull() const;
        Geometry* intersection(const Geometry& geometry) const;
        Geometry* Union(const Geometry& geometry) const;
        Geometry* difference(const Geometry& geometry) const;
        Geometry* symDifference(const Geometry& geometry) const;

    private:
        Geometry(geos::geom::Geometry* geometry, const Ilwis::ICoordinateSystem& csy);
        const std::unique_ptr<geos::geom::Geometry>& ptr() const;
        bool _standalone;

        //_standalone == false
        std::unique_ptr<Feature> _feature;
        QVariant _index;

        //_standalone == true
        std::unique_ptr<geos::geom::Geometry> _ilwisGeometry;
    };

} // namespace pythonapi

#endif // PYTHONAPI_GEOMETRY_H
