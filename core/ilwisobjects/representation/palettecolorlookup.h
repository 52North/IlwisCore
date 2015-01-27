#ifndef PALETTECOLORLOOKUP_H
#define PALETTECOLORLOOKUP_H

#include "colorlookup.h"
#include "boost/container/flat_map.hpp"

namespace Ilwis {
class KERNELSHARED_EXPORT PaletteColorLookUp : public ColorLookUp{

public:
    PaletteColorLookUp(const QString &definition);
    PaletteColorLookUp(boost::container::flat_map<quint32, QColor>& newcolors);
    QColor value2color(double index, const NumericRange& actualRange = NumericRange(), const NumericRange& stretchRange = NumericRange()) const;

private:
    boost::container::flat_map<quint32, QColor> _colors;
    bool _cyclic = true;
    int _cycleCount = 16;
};
}

#endif // PALETTECOLORLOOKUP_H
