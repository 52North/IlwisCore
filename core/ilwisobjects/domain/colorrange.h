#ifndef COLORRANGE_H
#define COLORRANGE_H

namespace Ilwis{

class KERNELSHARED_EXPORT ColorRange : public Range
{
public:
    enum ColorModel{cmRGBA, cmHSLA, cmCYMKA, cmGREYSCALE};

    ColorRange(IlwisTypes tp, ColorModel clrmodel);
    IlwisTypes valueType() const;
    ColorModel defaultColorModel() const;
    void defaultColorModel(ColorModel m);
    virtual bool contains(ColorRange *v, bool inclusive = true) const = 0;
    virtual bool contains(const QVariant& v, bool inclusive = true) const = 0;

    static QColor toColor(quint64 clrint, ColorModel clrModel) ;
    static QColor toColor(const QVariant &v, ColorModel colormodel);
    static QString toString(const QColor& clr, ColorModel clrModel);


protected:
private:
    IlwisTypes _valuetype;
    ColorModel _defaultModel = cmRGBA;

};

class KERNELSHARED_EXPORT ContinousColorRange  : public ColorRange{
public:
    ContinousColorRange();
    ContinousColorRange(const QColor& clr1=QColor("#000000"), const QColor& clr2=QColor("(#ffffff"), ColorModel colormodel=ColorRange::cmRGBA);
    bool isValid() const;
    QString toString() const;
    Range *clone() const;
    QVariant ensure(const QVariant& v, bool inclusive = true) const;
    bool contains(const QVariant& v, bool inclusive = true) const;
    bool contains(ColorRange *v, bool inclusive = true) const;
    QVariant impliedValue(const QVariant& v) const;
private:
    QColor _limit1;
    QColor _limit2;
};
class KERNELSHARED_EXPORT PallettedColorRange : public ColorRange {
public:
    PallettedColorRange();
    PallettedColorRange(const std::vector<QColor>& colors, ColorModel clrmodel=ColorRange::cmRGBA );
    bool isValid() const;
    QString toString() const;
    Range *clone() const;
    QVariant ensure(const QVariant& v, bool inclusive = true) const;
    bool contains(const QVariant& v, bool inclusive = true) const;
    bool contains(ColorRange *v, bool inclusive = true) const;
    QVariant impliedValue(const QVariant& v) const;
    void addColor(const QColor& clr);
    QColor color(quint32 index) const;
    quint32 palletSize() const;

private:
    std::vector<QColor> _pallet;
};

}

#endif // COLORRANGE_H
