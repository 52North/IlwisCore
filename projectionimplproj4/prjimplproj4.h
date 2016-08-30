#ifndef PROJECTIONIMPLEMENTATIONPROJ4_H
#define PROJECTIONIMPLEMENTATIONPROJ4_H

namespace Ilwis {
class ProjectionImplementationProj4 : public ProjectionImplementation
{
public:
    ProjectionImplementationProj4(const Ilwis::Resource &resource);
     ProjectionImplementationProj4(const QString& projtype,const QString &code);
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
     bool canConvertToLatLon() const;
     bool canConvertToCoordinate() const;
     virtual ProjectionImplementation* clone();
 protected:
     virtual void copyTo(ProjectionImplementation *prj);
private:
    ProjectionImplementationProj4();
    QString _targetDef;
    projPJ  _pjLatlon;
    projPJ  _pjBase;
    bool _outputIsLatLon;
    void fillParameters(const QString &code);
    void removeParameter(const QString &parm);
};
}

#endif // PROJECTIONIMPLEMENTATIONPROJ4_H
