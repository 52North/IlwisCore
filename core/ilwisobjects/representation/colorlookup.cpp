#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "itemrange.h"
#include "colorrange.h"
#include "numericrange.h"
#include "datadefinition.h"
#include "colorlookup.h"

using namespace Ilwis;

std::vector<QColor> ColorLookUp::values2colors(const std::vector<double> &values, const NumericRange &actualRange, const NumericRange &stretchRange) const
{
    std::vector<QColor> colors(values.size());
    auto iter = colors.begin();
    for(auto value : values){
        *iter = value2color(value, actualRange, stretchRange);
        ++iter;
    }
    return colors;
}




