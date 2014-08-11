#ifndef COLORRANGE_H
#define COLORRANGE_H

#include <QColor>

namespace Ilwis{

class ColorItem;
typedef QSharedPointer<ColorItem> SPColorItem;

struct KERNELSHARED_EXPORT LocalColor {
    quint8 _component1=0;
    quint8 _component2=0;
    quint8 _component3=0;
    quint8 _component4=0;
} ;


class KERNELSHARED_EXPORT ColorRangeBase
{
public:
    enum ColorModel{cmNONE, cmRGBA, cmHSLA, cmCYMKA, cmGREYSCALE};

    ColorRangeBase(IlwisTypes tp, ColorModel clrmodel);
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

class KERNELSHARED_EXPORT ContinousColorRange  : public Range, public ColorRangeBase{
public:
    ContinousColorRange();
    ContinousColorRange(const QColor& clr1, const QColor& clr2=QColor("#FFFFFF"), ColorModel colormodel=ColorRangeBase::cmRGBA);
    bool isValid() const;
    QString toString() const;
    Range *clone() const;
    QVariant ensure(const QVariant& v, bool inclusive = true) const;
    bool contains(const QVariant& v, bool inclusive = true) const;
    bool contains(const Range *v, bool inclusive = true) const;
    QVariant impliedValue(const QVariant& v) const;
    IlwisTypes valueType() const;
    static QColor valueAt(quint32& index, const Ilwis::Range *rng);
    quint32 count() const;
    void add(const QVariant& v);
private:
    QColor _limit1;
    QColor _limit2;
};

class KERNELSHARED_EXPORT ColorPalette : public ItemRange, public ColorRangeBase  {
public:
    ColorPalette();
    QVariant impliedValue(const QVariant& v) const;

    SPDomainItem item(quint32 raw) const;
    SPDomainItem item(const QString &nam) const;
    SPDomainItem itemByOrder(quint32 index) const;
    QColor color(int index) const;
    QString toString() const;
    Range *clone() const;

    void add(DomainItem *item);
    void add(SPDomainItem item);
    void add(const QVariant& color);
    void remove(const QString& nm);
    void clear();

    bool contains(const QVariant& color, bool inclusive = true) const;
    bool contains(SPRange rng, bool inclusive=true) const;
    bool contains(Ilwis::Range *rng, bool inclusive=true) const;

    QVariant ensure(const QVariant& v, bool inclusive = true) const;
    quint32 count() const;
    IlwisTypes valueType() const;
    static QColor valueAt(quint32& index, const Ilwis::Range *rng);

    bool isValid() const;

    qint32 gotoIndex(qint32 index, qint32 step) const;
    bool alignWithParent(const IDomain &dom);
private:
    std::vector<SPColorItem> _colors;

};


}

#endif // COLORRANGE_H
