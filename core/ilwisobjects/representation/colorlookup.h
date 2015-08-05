#ifndef COLORLOOKUP_H
#define COLORLOOKUP_H

namespace Ilwis {

class Domain;
typedef IlwisData<Domain> IDomain;


class NumericRange;

class KERNELSHARED_EXPORT ColorLookUp
{
public:
    virtual QColor value2color(double value, const NumericRange& actualRange = NumericRange(), const NumericRange& stretchRange = NumericRange()) const = 0;
    virtual void setColor(double value, const QColor& clr)  = 0;
    std::vector<QColor> values2colors(const NumericRange &actualRange, const NumericRange &stretchRange, int paletteSize=256) const;
    virtual ColorLookUp *clone() const = 0;
    static ColorLookUp *create(const Ilwis::IDomain &dom, const QString &rprCode="");
protected:
    QColor string2color(const QString &colorstring);
};




}

#endif // COLORLOOKUP_H
