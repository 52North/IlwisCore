#ifndef PLATECAREE_H
#define PLATECAREE_H

namespace Ilwis {
namespace Internal {
class PlateCaree : public ProjectionImplementationInternal
{
public:
    PlateCaree(const Ilwis::Resource &item);
    ~PlateCaree();
    Coordinate ll2crd(const LatLon&) const;
    LatLon crd2ll(const Coordinate&) const;
    static bool canUse(const Ilwis::Resource &) ;
    bool prepare(const QString &parms = "");
};
}
}

#endif // PLATECAREE_H
