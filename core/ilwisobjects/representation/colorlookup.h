#ifndef COLORLOOKUP_H
#define COLORLOOKUP_H

namespace Ilwis {

class NumericRange;

class KERNELSHARED_EXPORT ColorLookUp
{
public:
    virtual QColor value2color(double value, const NumericRange& actualRange = NumericRange(), const NumericRange& stretchRange = NumericRange()) const = 0;
    std::vector<QColor> values2colors(const std::vector<double>& values, const NumericRange& actualRange = NumericRange(), const NumericRange& stretchRange = NumericRange()) const;
};




}

#endif // COLORLOOKUP_H
