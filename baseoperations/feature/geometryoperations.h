#ifndef WITHIN_H
#define WITHIN_H

namespace Ilwis{
class Within  : public GeometryMethod, public BoolFGeomGeom
{
public:
    Within();

    bool execute(bool& outBool,const Geometry& container, const Geometry& containee=Geometry()) const;
    GeometryMethod *create() const;
};

class Area  : public GeometryMethod, public DoubleFGeomGeom
{
public:
    Area();

    bool execute(double& outval,const Geometry& geom1,const Geometry& geom2=Geometry()) const;
    GeometryMethod *create() const;
};

class Centroid  : public GeometryMethod, public GeomFGeomGeom
{
public:
    Centroid();

    bool execute(Ilwis::Geometry &outGeom, const Geometry& geom1,const Geometry& geom2=Geometry()) const;
    GeometryMethod *create() const;
};

class ConvexHull  : public GeometryMethod, public GeomFGeomGeom
{
public:
    ConvexHull();

    bool execute(Ilwis::Geometry &outGeom, const Geometry& geom,const Geometry& geom2=Geometry()) const;
    GeometryMethod *create() const;
};

//------------------------------------------------
}

#endif // WITHIN_H
