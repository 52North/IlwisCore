#ifndef GEOMETRYHELPER_H
#define GEOMETRYHELPER_H

namespace geos {
namespace geom {
    class Geometry;
}
namespace io{
    class ParseException;
}
}

namespace Ilwis {


class KERNELSHARED_EXPORT GeometryHelper{
    public:
        static QString toWKT(const geos::geom::Geometry* geom);
        static geos::geom::Geometry* fromWKT(const QString &wkt, const Ilwis::ICoordinateSystem &csy);

        static IlwisTypes geometryType(const geos::geom::Geometry* geom);

        static void transform(geos::geom::Geometry* geom, const ICoordinateSystem& source, const ICoordinateSystem& target);

        static Ilwis::CoordinateSystem* getCoordinateSystem(geos::geom::Geometry* geom);

        static void setCoordinateSystem(geos::geom::Geometry* geom, Ilwis::CoordinateSystem* csy);
};

}

#endif // GEOMETRYHELPER_H
