#ifndef PALETTECOLORLOOKUP_H
#define PALETTECOLORLOOKUP_H

#include "colorlookup.h"
#include "boost/container/flat_map.hpp"

namespace Ilwis {
class KERNELSHARED_EXPORT PaletteColorLookUp : public ColorLookUp{

public:
    PaletteColorLookUp();
    PaletteColorLookUp(const QString &definition);
    PaletteColorLookUp(const Ilwis::IDomain &dom, const QString& rprCode= "");
    PaletteColorLookUp(boost::container::flat_map<quint32, QColor>& newcolors);
    QColor value2color(double index, const NumericRange& actualRange = NumericRange(), const NumericRange& stretchRange = NumericRange()) const;
    void setColor(double value, const QColor &clr);

    ColorLookUp *clone() const;
private:
    boost::container::flat_map<quint32, QColor> _colors;
    bool _cyclic = true;
    void fromDefinition(const QString &definition, const Ilwis::IDomain &dom = IDomain());
};
}

#endif // PALETTECOLORLOOKUP_H
