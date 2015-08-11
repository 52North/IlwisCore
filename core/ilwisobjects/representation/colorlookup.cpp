#include <sstream>
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

using namespace Ilwis;

std::vector<QColor> ColorLookUp::values2colors(const NumericRange &actualRange, const NumericRange &stretchRange, int paletteSize) const
{
    std::vector<QColor> colors(paletteSize);
    colors[0] = QColor("transparent");
    for(int i=1; i < paletteSize; ++i){
        double value = actualRange.min() + actualRange.distance() * ((double) (i-1)/ (paletteSize - 1));
        colors[i] = value2color(value, actualRange, stretchRange);
    }
    return colors;
}

ColorLookUp *ColorLookUp::create(const IDomain &dom, const QString& rprCode)
{
    if ( hasType(dom->ilwisType(), itITEMDOMAIN | itTEXTDOMAIN)){
        return new PaletteColorLookUp(dom, rprCode);
    }else if ( hasType(dom->ilwisType(),itNUMERICDOMAIN)){
        return new ContinuousColorLookup(dom, rprCode);
    }
    return 0;
}

QColor ColorLookUp::string2color(const QString& colorstring)
{
    QColor color;
    if ( colorstring.size() > 7){
        unsigned long val;
        std::stringstream hexs;
        hexs << std::hex << colorstring.mid(1,2).toStdString();
        hexs >> val;
        QString restColor = "#" + colorstring.mid(3);
        color = QColor(restColor);
        color.setAlpha(val);
    }else
        color = QColor(colorstring);
    return color;
}




