#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "itemrange.h"
#include "colorrange.h"
#include "numericrange.h"
#include "datadefinition.h"
#include "palettecolorlookup.h"

using namespace Ilwis;

PaletteColorLookUp::PaletteColorLookUp(const QString &definition){
    QStringList parts = definition.split(";");
    for( QString group : parts){
        QStringList groupdef = group.split("|");
        if ( groupdef.size() != 5) {
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }
        QStringList limits = groupdef[0].split(":");
        if ( limits.size() != 2 && limits.size() != 3){
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }
        bool ok1, ok2;
        NumericRange numrange(limits[0].toDouble(&ok1), limits[1].toDouble(&ok2));
        if ( !(ok1 && ok2)){
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }
        QColor color1(groupdef[1]);
        if ( !(color1.isValid())){
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }
        double  transparency = groupdef[2].toDouble(&ok1);
        if ( !ok1){
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }
        color1.setAlphaF(transparency);

        QColor color2(groupdef[3]);
        if ( !(color2.isValid())){
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }
        transparency = groupdef[4].toDouble(&ok1);
        if ( !ok1){
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }

        if ( numrange.min() == 0 && numrange.max() == 0){
            for(int i = 0; i < _cycleCount ; ++i) {
                ContinuousColorRange colorrange(color1, color2);
                double index  = (double)i/_cycleCount;
                QColor color = ContinuousColorRange::valueAt(index,&colorrange);
                _colors[i] = color;
            }
        }

    }
}

PaletteColorLookUp::PaletteColorLookUp(boost::container::flat_map<quint32, QColor> &newcolors) : _colors(newcolors)
{

}

QColor PaletteColorLookUp::value2color(double index, const NumericRange &rng, const NumericRange &) const
{
    if ( index == 0)
        return QColor(255,0,0);
    if ( index == 1)
        return QColor(0,255,0);
    if (index == 2)
        return QColor(0,0,255);
    int localIndex  = index;
    if ( _cyclic){
        localIndex = _cycleCount * index / rng.count();
    }
    auto iter = _colors.find(localIndex);
    if ( iter != _colors.end()){

        QColor clr =  (*iter).second;
        qDebug() << clr << index << localIndex;
        return clr;
    }
    return QColor();
}
