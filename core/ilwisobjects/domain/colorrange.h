#ifndef COLORRANGE_H
#define COLORRANGE_H

#include <QColor>

namespace Ilwis{

class ColorItem;
typedef QSharedPointer<ColorItem> SPColorItem;

class KERNELSHARED_EXPORT ColorRange : public Range
{
public:
    enum ColorModel{cmNONE, cmRGBA, cmHSLA, cmCYMKA, cmGREYSCALE};

    ColorRange(IlwisTypes tp, ColorModel clrmodel);
    ColorModel defaultColorModel() const;
    void defaultColorModel(ColorModel m);

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
    ContinousColorRange(const QColor& clr1, const QColor& clr2=QColor("#FFFFFF"), ColorModel colormodel=ColorRange::cmRGBA);
    bool isValid() const;
    QString toString() const;
    Range *clone() const;
    QVariant ensure(const QVariant& v, bool inclusive = true) const;
    bool contains(const QVariant& v, bool inclusive = true) const;
    bool contains(ColorRange *v, bool inclusive = true) const;
    QVariant impliedValue(const QVariant& v) const;
    IlwisTypes valueType() const;
    QColor valueAt(quint32& index, const Ilwis::Range *rng);
    quint32 count() const;
private:
    QColor _limit1;
    QColor _limit2;
};

class ColorPalette : public ItemRange, public ColorRange  {
public:
    QVariant impliedValue(const QVariant& v) const;

    SPDomainItem item(quint32 raw) const;
    SPDomainItem item(const QString &nam) const;
    SPDomainItem itemByOrder(quint32 index) const;
    QColor color(int index) const;

    void add(DomainItem *item);
    void add(SPDomainItem item);
    void remove(const QString& nm);
    void clear();

    bool contains(const QVariant& color, bool inclusive = true) const;
    bool contains(SPRange rng, bool inclusive=true) const;
    bool contains(ItemRange *rng, bool inclusive=true) const;

    QVariant ensure(const QVariant& v, bool inclusive = true) const;
    quint32 count() const;
    IlwisTypes valueType() const;

    bool isValid() const;

private:
    std::vector<SPColorItem> _colors;

};


}

#endif // COLORRANGE_H
