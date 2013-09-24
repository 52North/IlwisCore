#ifndef PROJECTIONIMPLEMENTATIONPROJ4_H
#define PROJECTIONIMPLEMENTATIONPROJ4_H

namespace Ilwis {
class ProjectionImplementationProj4 : public ProjectionImplementation
{
public:
    ProjectionImplementationProj4(const Ilwis::Resource &resource);
    ~ProjectionImplementationProj4();
    Coordinate latlon2coord(const LatLon&) const;
    LatLon coord2latlon(const Coordinate&) const;
    static bool canUse(const Ilwis::Resource &) { return true;}
    static ProjectionImplementation *create(const Ilwis::Resource &resource);
     bool compute() { return true; }
     void setParameter(Projection::ProjectionParamValue type, const QVariant& value);
     void setEllipsoid(const Ilwis::IEllipsoid &ell);
     bool prepare(const QString& parms="");
     QString toProj4() const;
private:
    QString _targetDef;
    projPJ  _pjLatlon;
    projPJ  _pjBase;
    bool _outputIsLatLon;
};
}

#endif // PROJECTIONIMPLEMENTATIONPROJ4_H
