#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "itemrange.h"
#include "colorrange.h"
#include "numericrange.h"
#include "colorlookup.h"

using namespace Ilwis;

std::vector<QColor> ColorLookUp::values2colors(const std::vector<double> &values,const NumericRange& actualRange, const NumericRange &stretchRange) const
{
    std::vector<QColor> colors(values.size());
    auto iter = colors.begin();
    for(auto value : values){
        *iter = value2color(value, actualRange, stretchRange);
        ++iter;
    }
    return colors;
}
//----------------------------------------------------------
PaletteLookUp::PaletteLookUp(boost::container::flat_map<quint32, QColor> &newcolors) : _colors(newcolors)
{

}

QColor PaletteLookUp::value2color(double index, const NumericRange& , const NumericRange &) const
{
    auto iter = _colors.find((quint32)index);
    if ( iter != _colors.end())
        return (*iter).second;
    return QColor();
}

//-----------------------------------------------------------
ContinuousLookUp::ContinuousLookUp(const QString &definition)
{
    QStringList parts = definition.split(";");
    for( QString group : parts){
        QStringList groupdef = group.split("|");
        if ( groupdef.size() != 9) {
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }
        QStringList limits = groupdef[0].split(":");
        if ( limits.size() != 2){
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }
        bool ok1, ok2, ok3, ok4;
        NumericRange numrange(limits[0].toDouble(&ok1), limits[1].toDouble(&ok2));
        if ( !(ok1 && ok2)){
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }
        QColor color1(groupdef[1].toInt(&ok1), groupdef[2].toInt(&ok2), groupdef[3].toInt(&ok3), groupdef[4].toInt(&ok4));
        if ( !(ok1 && ok2 && ok3 && ok4)){
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }
        QColor color2(groupdef[5].toInt(&ok1), groupdef[6].toInt(&ok2), groupdef[7].toInt(&ok3), groupdef[8].toInt(&ok4));
        if ( !(ok1 && ok2 && ok3 && ok4)){
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }
        ContinuousColorRange colorrange(color1, color2);
        addGroup(numrange,colorrange);

    }


}

QColor ContinuousLookUp::value2color(double value, const NumericRange& actualRange, const NumericRange &stretchRange) const
{
    if ( stretchRange.isValid())  {
        if ( _linear){
            if ( value < stretchRange.center()){
                double stretchFraction = (value - stretchRange.min())/ stretchRange.distance();
                value = actualRange.min() + stretchFraction * actualRange.distance();
            }else{
                if ( value >= stretchRange.center()){
                    double stretchFraction = (stretchRange.max() - value)/ stretchRange.distance();
                    value = actualRange.max() - stretchFraction * actualRange.distance();
                }
            }
        }
    }
    if ( value > _numericRange.max())
        return QColor();
    if ( value < _numericRange.min())
        return QColor();

    value = value - actualRange.min() / actualRange.distance(); // scale it between 0..1
    for(int i = 0; i < _groups.size(); ++i){
        if ( _groups[i].min() < value){
            double delta = _groups[i - 1].distance();
            double position = 0;
            if ( _step == 0){
                position = (value - _groups[i - 1].min())/ delta;
            }else
                position = ((quint32)(value - _groups[i - 1].min())/ _step)/( (quint32)(delta / _step));

            return ContinuousColorRange::valueAt(position,&_colorranges[i-1]);
        }
    }
    return QColor();
}

void ContinuousLookUp::addGroup(const NumericRange &range, const ContinuousColorRange &colorrange)
{
    if ( !(range.min() >= 0 && range.max() <= 1.0)){
        ERROR2(ERR_INVALID_INIT_FOR_2, TR("Numerical range"), "Representation");
        return;
    }
    if ( _colorranges.size() > 0){
        if ( range.min() < _groups.back().min()){
            kernel()->issues()->log(TR("Numerical ranges for representation must be added in order"));
            return ;
        }
    }
    _colorranges.push_back(colorrange);
    _groups.push_back(range);
}



