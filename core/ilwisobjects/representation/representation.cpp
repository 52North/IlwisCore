#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "itemrange.h"
#include "colorrange.h"
#include "numericrange.h"
#include "datadefinition.h"
#include "colorlookup.h"
#include "continuouscolorlookup.h"
#include "palettecolorlookup.h"
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

bool Representation::isCompatible(const IDomain &otherDomain)
{
    if ( domain().isValid() && otherDomain.isValid()){
        if ( otherDomain == domain())
            return true;

        if ( hasType(domain()->ilwisType(), itNUMBER) && hasType(otherDomain->ilwisType(), itNUMBER)){
            return true;
        }
        //TODO other cases
    }

    return false;
}

IlwisTypes Representation::ilwisType() const
{
    return itREPRESENTATION;
}

IlwisData<Representation> Representation::defaultRepresentation(const IDomain &dom)
{
    QString code("code=rpr:monochromeblack"); // default
    if ( hasType(dom->ilwisType(), itNUMERICDOMAIN)){
        NumericRange *numrange = dom->range()->as<NumericRange>();
        if ( numrange->min() >= 0 && numrange->max() < 256 && numrange->resolution() == 1)
            code = "code=rpr:grey";
        else
            code = "code=rpr:pseudo";
    }else if ( hasType(dom->ilwisType(), itITEMDOMAIN)){
        code = "code=rpr:primarycolors";
    }

    return IRepresentation(code);
}
