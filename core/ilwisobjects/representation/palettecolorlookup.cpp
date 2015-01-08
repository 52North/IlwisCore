#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "itemrange.h"
#include "colorrange.h"
#include "numericrange.h"
#include "datadefinition.h"
#include "palettecolorlookup.h"

using namespace Ilwis;

PaletteColorLookUp::PaletteColorLookUp(boost::container::flat_map<quint32, QColor> &newcolors) : _colors(newcolors)
{

}

QColor PaletteColorLookUp::value2color(double index, const NumericRange &actualRange, const NumericRange &stretchRange) const
{
    auto iter = _colors.find((quint32)index);
    if ( iter != _colors.end())
        return (*iter).second;
    return QColor();
}
