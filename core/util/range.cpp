#include <QString>
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "range.h"
#include "numericrange.h"
#include "itemrange.h"
#include "identifierrange.h"
#include "interval.h"
#include "intervalrange.h"
#include "juliantime.h"
#include "geometries.h"
#include "box.h"
#include "colorrange.h"

using namespace Ilwis;

Range::Range()
{
}

bool Range::isContinuous() const
{
    return _interpolation.size() != 0;
}

QString Range::interpolation() const
{
    return _interpolation;
}

void Range::interpolation(const QString &ip)
{
    _interpolation = ip;
}

Range *Range::create(IlwisTypes types) {

    switch (types){
    case itNAMEDITEM:
        return new NamedIdentifierRange();
    case itTHEMATICITEM:
        return new ThematicRange();
    case itINDEXEDITEM:
        return new IndexedIdentifierRange();
    case itNUMERICITEM:
        return new IntervalRange();
    case itTIME:
        return new TimeInterval();
    case itENVELOPE:
        return new Envelope();
    case itBOUNDINGBOX:
        return new BoundingBox();
    case itCOLOR:
        return new ContinuousColorRange();
    case itPALETTECOLOR:
        return new ColorPalette();
    default:
        if ( hasType(types, itNUMBER)){
            return new NumericRange();
        }
    }
    return 0;
}
