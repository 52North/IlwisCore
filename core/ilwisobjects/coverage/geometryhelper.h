#ifndef GEOMETRYHELPER_H
#define GEOMETRYHELPER_H

#include <complex>
#include "ilwisangle.h"

namespace geos {
namespace geom {
    class Geometry;
}
namespace io{
    class ParseException;
}
}

namespace Ilwis {

class CoordinateSystem;
typedef IlwisData<CoordinateSystem> ICoordinateSystem;

class KERNELSHARED_EXPORT GeometryHelper{
    public:
        static QString toWKT(const geos::geom::Geometry* geom);
        static geos::geom::Geometry* fromWKT(const QString &wkt, const Ilwis::ICoordinateSystem &csy);

        static IlwisTypes geometryType(const geos::geom::Geometry* geom);

        static void transform(geos::geom::Geometry* geom, const ICoordinateSystem& source, const ICoordinateSystem& target);

        static Ilwis::CoordinateSystem* getCoordinateSystem(geos::geom::Geometry* geom);

        static void setCoordinateSystem(geos::geom::Geometry* geom, Ilwis::CoordinateSystem* csy);

        template<typename PointType> static std::vector<PointType> rotate2d(const PointType &center, const Angle& angle, const std::vector<PointType>& inputPoints)
        {
            typedef std::complex<double> CPoint;
            CPoint about(center.x, center.y);
            std::vector<PointType> outputPoints;
            for(auto p : inputPoints){
                if ( p.isValid()){
                    CPoint outPoint = (CPoint(p.x,p.y) - about) * exp(CPoint(0, angle.radians())) + about;
                    outputPoints.push_back(PointType(outPoint.real(), outPoint.imag()));
                }else
                    outputPoints.push_back(PointType());
            }
            return outputPoints;
        }
        static std::vector<geos::geom::CoordinateSequence *> geometry2coords(const geos::geom::Geometry *geom);
};

typedef std::unique_ptr<geos::geom::Geometry> UPGeometry;
}

#endif // GEOMETRYHELPER_H
