#ifndef PLATECAREE_H
#define PLATECAREE_H

namespace Ilwis {
namespace Internal {
class PlateCaree : public ProjectionImplementationInternal
{
public:
    PlateCaree(const Ilwis::Resource &resource);
    ~PlateCaree();
    Coordinate ll2crd(const LatLon&) const;
    LatLon crd2ll(const Coordinate&) const;
    static bool canUse(const Ilwis::Resource &) ;
    bool prepare(const QString &parms = "");
    ProjectionImplementation *clone();
private:
    PlateCaree();
};
}
}

#endif // PLATECAREE_H
