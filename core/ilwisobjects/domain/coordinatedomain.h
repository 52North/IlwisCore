#ifndef COORDINATEDOMAIN_H
#define COORDINATEDOMAIN_H

namespace Ilwis {
class CoordinateDomain : public Domain
{
public:
    CoordinateDomain();
    CoordinateDomain(const Resource& res);

    void setRange(Range *vr);
    Domain::Containement contains(const QVariant& value) const;
    QString value(const QVariant& v) const;

    IlwisTypes valueType() const;
    IlwisTypes ilwisType() const;
    ICoordinateSystem coordinateSystem() const;

protected:
    SPRange getRange() const;
private:
    ICoordinateSystem _csy;
    SPRange _envelope;
};
}

typedef Ilwis::IlwisData<Ilwis::CoordinateDomain> ICoordinateDomain;
#endif // COORDINATEDOMAIN_H
