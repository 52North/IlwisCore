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

bool Coverage::prepare(const IOOptions &options) {

    bool ok = IlwisObject::prepare(options);

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
    resourceRef().addProperty("coordinatesystem",coordinateSystem()->id());
    mastercatalog()->changeResource(id(),"coordinatesystem",csy->id(), true);
}

Envelope Coverage::envelope(bool tolatlon) const
{
    if ( !coordinateSystem().isValid())
        return Envelope();

    if ( tolatlon){
        if ( _coordinateSystem->isLatLon())
            return _envelope;
        else if (_coordinateSystem->canConvertToLatLon()){
            auto env = Envelope(_coordinateSystem->coord2latlon(_envelope.min_corner()),
                                _coordinateSystem->coord2latlon(_envelope.max_corner()));
            return env;
        }else{
            return Envelope();
        }
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

Resource Coverage::resource(int mode) const
{
    Resource resource = IlwisObject::resource(mode);
    if ( mode & IlwisObject::cmEXTENDED) {
        resource.addProperty("coordinatesystem", coordinateSystem()->id());
        resource.setExtendedType( resource.extendedType() | itCOORDSYSTEM);
    }
    return resource;
}

bool Coverage::canUse(const IlwisObject *obj, bool strict) const
{
    if ( IlwisObject::canUse(obj, strict))
        return true;

    if ( hasType(obj->ilwisType(), itCOORDSYSTEM )){
        if ( strict ){
            if ( coordinateSystem()->isCompatibleWith(obj))
                return true;
        }else
            return true;
    }
    return false;
}

void Coverage::name(const QString &nam)
{
    IlwisObject::name(nam)    ;
}

bool Coverage::hasAttributes() const
{
    return true;
}

void Coverage::copyTo(IlwisObject *obj)
{
    IlwisObject::copyTo(obj);
    Coverage *cov = static_cast<Coverage *>(obj);
    cov->_coordinateSystem = _coordinateSystem;
    cov->_envelope = _envelope;
    cov->_statistics = _statistics;
}


