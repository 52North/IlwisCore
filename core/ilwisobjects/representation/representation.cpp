#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "itemrange.h"
#include "colorrange.h"
#include "numericrange.h"
#include "colorlookup.h"
#include "representation.h"

using namespace Ilwis;

Representation::Representation()
{
}

Representation::Representation(const Resource &resource) : IlwisObject(resource)
{

}

const UPColorLookUp &Representation::colors() const
{
    if ( _colors){
        return _colors;
    }
    throw ErrorObject(TR("Using uninitialized representation"));

}

void Representation::colors(ColorLookUp *lookup)
{
    if (isReadOnly())
        return;
    changed(true);

    _colors.reset(lookup);
}

IDomain Representation::domain() const
{
    return _domain;
}

void Representation::domain(const IDomain &domain)
{
    if (isReadOnly())
        return;
    changed(true);

    _domain = domain;
}

IlwisTypes Representation::ilwisType() const
{
    return itREPRESENTATION;
}
