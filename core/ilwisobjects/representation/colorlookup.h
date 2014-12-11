#ifndef COLORLOOKUP_H
#define COLORLOOKUP_H
#include "boost/container/flat_map.hpp"

namespace Ilwis {
class KERNELSHARED_EXPORT ColorLookUp
{
public:
    virtual QColor value2color(double value, const NumericRange& actualRange = NumericRange() , const NumericRange& stretchRange = NumericRange()) const = 0;
    std::vector<QColor> values2colors(const std::vector<double>& values, const NumericRange& actualRange = NumericRange(), const NumericRange& stretchRange = NumericRange()) const;
};

class KERNELSHARED_EXPORT PaletteLookUp : public ColorLookUp{

public:
    PaletteLookUp(boost::container::flat_map<quint32, QColor>& newcolors);
    QColor value2color(double index, const NumericRange& = NumericRange(), const NumericRange& = NumericRange()) const;

private:
    boost::container::flat_map<quint32, QColor> _colors;
};

class KERNELSHARED_EXPORT ContinuousLookUp : public ColorLookUp{

public:
    ContinuousLookUp(const QString& definition);
    QColor value2color(double value, const NumericRange& actualRange = NumericRange(), const NumericRange& stretchRange = NumericRange()) const;
    void addGroup(const NumericRange& range, const ContinuousColorRange& colorrange);

private:
    std::vector<NumericRange> _groups;
    std::vector<ContinuousColorRange> _colorranges;
    NumericRange _numericRange;
    double _step = 0;
    bool _linear = true;
};
}

#endif // COLORLOOKUP_H
