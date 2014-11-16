#include "coverage.h"
#include "table.h"

using namespace Ilwis;

Coverage::Coverage()
{
}

Coverage::Coverage(const Resource &resource) : IlwisObject(resource)
{
}

Coverage::~Coverage()
{
    _coordinateSystem.set(0);
}

bool Coverage::prepare( ) {

    bool ok = IlwisObject::prepare();

    return ok;
}

ICoordinateSystem Coverage::coordinateSystem() const
{
    return _coordinateSystem;
}

void Coverage::coordinateSystem(const ICoordinateSystem &csy)
{
    if ( isReadOnly())
        return;
    changed(true);

    _coordinateSystem = csy;
}

Envelope Coverage::envelope(bool tolatlon) const
{
    if ( !coordinateSystem().isValid())
        return _envelope;

    if ( tolatlon && !coordinateSystem()->isLatLon() && code() != "csy:unknown"){
        bool validbounds = _envelope.min_corner().isValid() && _envelope.max_corner().isValid();
        if ( !validbounds)
            return Envelope();
        LatLon c1 = coordinateSystem()->coord2latlon(_envelope.min_corner());
        LatLon c2 = coordinateSystem()->coord2latlon(_envelope.max_corner());
        return Envelope(c1,c2);
    }
    return _envelope;
}

void Coverage::envelope(const Envelope &bnds)
{
    if ( isReadOnly())
        return;
    changed(true);

    _envelope = bnds;
}


NumericStatistics &Coverage::statistics(int  )
{
        return _statistics;
}

Resource Coverage::source(int mode) const
{
    Resource resource = IlwisObject::source(mode);
    if ( mode & IlwisObject::cmEXTENDED) {
        resource.addProperty("coordinatesystem", coordinateSystem()->id());
        resource.setExtendedType( resource.extendedType() | itCOORDSYSTEM);
    }
    return resource;
}

void Coverage::name(const QString &nam)
{
    Identity::name(nam)    ;
}

void Coverage::copyTo(IlwisObject *obj)
{
    IlwisObject::copyTo(obj);
    Coverage *cov = static_cast<Coverage *>(obj);
    cov->_coordinateSystem = _coordinateSystem;
    cov->_envelope = _envelope;
    cov->_statistics = _statistics;
}


