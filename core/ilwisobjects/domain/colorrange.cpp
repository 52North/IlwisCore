#include <QColor>
#include <QRegularExpression>
#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "domainitem.h"
#include "domain.h"
#include "itemrange.h"
#include "coloritem.h"
#include "itemiterator.h"
#include "colorrange.h"

using namespace Ilwis;

//const quint64 colormask1 = 0xF000000000000000;
//const quint64 colormask2 = 0xF00000000000;
//const quint64 colormask3 = 0xF0000000;
//const quint64 colormask4 = 0xF000;

struct LocalColor{
    quint16 _component1;
    quint16 _component2;
    quint16 _component3;
    quint16 _component4;
};

ColorRange::ColorRange(IlwisTypes tp, ColorModel clrmodel) : _valuetype(tp), _defaultModel(clrmodel)
{
}

ColorRange::ColorModel ColorRange::defaultColorModel() const
{
    return _defaultModel;
}

void ColorRange::defaultColorModel(ColorRange::ColorModel m)
{
    _defaultModel = m;
}

QColor ColorRange::toColor(const QVariant &v, ColorRange::ColorModel colormodel)
{
    if ( v.type() == QMetaType::QColor)
        return QColor(v.value<QColor>());
    else if ( v.type() == QMetaType::QString){
        QStringList parts = v.toString().split(QRegularExpression("(|,|)]"));
        QColor clr;
        bool ok1,ok2,ok3, ok4, ok5=true;
        if ( parts.size() >= 5){
            double component1 = parts[1].toDouble(&ok1);
            double component2 = parts[2].toDouble(&ok2);
            double component3 = parts[3].toDouble(&ok3);
            double component4 = parts[4].toDouble(&ok4);
            double component5 =  parts.size()== 6 ? parts[5].toDouble(&ok5) : rUNDEF;
            if(! (ok1 && ok2 && ok3 && ok4 && ok5))
                return QColor();

            bool isFractional =  component1 <= 1 && component2 <= 1 && component3 <= 1 && component4 <= 1;
            if ( parts[0].toLower() == "rgba"){
                if ( isFractional){
                   clr.setRgbF(component1,component2, component3);
                   clr.setAlphaF(component4);
                }
                else{
                    clr.setRgb(component1,component2, component3);
                    clr.setAlpha(component4);
                }
            }else if ( parts[0].toLower() == "hsia"){
                if ( isFractional){
                   clr.setHslF(component1,component2, component3);
                   clr.setAlphaF(component4);
                }
                else{
                    clr.setHsl(component1,component2, component3);
                    clr.setAlpha(component4);
                }

            } else if ( parts[0].toLower() == "hsia" && parts.size() == 6){
                if ( isFractional){
                   clr.setCmykF(component1,component2, component3, component4);
                   clr.setAlphaF(component5);
                }
                else{
                    clr.setCmyk(component1,component2, component3, component4);
                    clr.setAlpha(component5);
                }
            }
            return clr;
        }
    } else if( v.type() == QMetaType::ULongLong){
        return ColorRange::toColor(v.toULongLong(),colormodel);
    }

    return QColor();
}

QString ColorRange::toString(const QColor &clr, ColorRange::ColorModel clrModel)
{
    QString color;
    switch(clrModel){
    case ColorRange::cmRGBA:
        color += QString("RGBA(%1,%2,%3,%4)").arg(clr.redF()).arg(clr.greenF()).arg(clr.blueF()).arg(clr.alphaF());
    case ColorRange::cmHSLA:
        color += QString("HSLA(%1,%2,%3,%4)").arg(clr.hueF()).arg(clr.saturationF()).arg(clr.lightnessF()).arg(clr.alphaF());
    case ColorRange::cmCYMKA:
        color += QString("CMYKA(%1,%2,%3,%4)").arg(clr.cyanF()).arg(clr.magentaF()).arg(clr.yellowF()).arg(clr.blackF()).arg(clr.alphaF());
    }
    return color;
}

QColor ColorRange::toColor(quint64 clrint, ColorModel clrModel)
{

    LocalColor *localcolor = reinterpret_cast<LocalColor *>(&clrint);

    QColor clr;
    if ( clrModel == ColorRange::cmRGBA){
        clr = QColor(localcolor->_component1,localcolor->_component2, localcolor->_component3);
        clr.setAlpha(localcolor->_component4);
    }
    else if ( clrModel ==  ColorRange::cmHSLA){
        clr.setHsl(localcolor->_component1,localcolor->_component2, localcolor->_component3);
        clr.setAlpha(localcolor->_component4);
    } else if ( clrModel ==  ColorRange::cmCYMKA){
        clr.setCmyk(localcolor->_component1,localcolor->_component2, localcolor->_component3, localcolor->_component4);
        //clr.setAlpha(localcolor->_component4); ??
    }
    return clr;
}

//----------------------------------------------------------------
ContinousColorRange::ContinousColorRange() : ColorRange(itCONTINUOUSCOLOR,ColorRange::cmRGBA)
{

}

ContinousColorRange::ContinousColorRange(const QColor &clr1, const QColor &clr2, ColorRange::ColorModel colormodel) : ColorRange(itCONTINUOUSCOLOR,colormodel), _limit1(clr1), _limit2(clr2)
{

}

bool ContinousColorRange::isValid() const
{
    return _limit1.isValid() && _limit2.isValid();
}

QString ContinousColorRange::toString() const
{
    if (!isValid())
        return sUNDEF;

    return ColorRange::toString(_limit1, defaultColorModel()) + "," + ColorRange::toString(_limit2, defaultColorModel());
}

Range *ContinousColorRange::clone() const
{
    return new ContinousColorRange(_limit1, _limit2, defaultColorModel());
}

QVariant ContinousColorRange::ensure(const QVariant &v, bool inclusive) const
{
    if ( contains(v,inclusive))
        return v;
    return QVariant();
}

bool ContinousColorRange::contains(const QVariant &v, bool inclusive) const
{
    QColor clr = v.value<QColor>();
    if ( !clr.isValid())
        return false;

    double component1a, component2a, component3a,component4a;
    double component1b, component2b, component3b,component4b;
    double component1c, component2c, component3c,component4c;
    QColor limit1,limit2;

    switch (defaultColorModel()) {
    case ColorRange::cmRGBA :
    case ColorRange::cmGREYSCALE:
        clr.getRgbF(&component1a, &component2a, &component3a);
        _limit1.getRgbF(&component1b, &component2b, &component3b);
        _limit2.getRgbF(&component1c, &component2c, &component3c);
        break;
    case ColorRange::cmHSLA :
        clr.getHslF(&component1a, &component2a, &component3a);
        _limit1.getHslF(&component1b, &component2b, &component3b);
        _limit2.getHslF(&component1c, &component2c, &component3c);
        break;
    case ColorRange::cmCYMKA:
        clr.getCmykF(&component1a, &component2a, &component3a,&component4a);
        limit1 = QColor(_limit1);
        limit2 = QColor(_limit2);
        limit1.getCmykF(&component1b, &component2b, &component3b, &component4b);
        limit2.getCmykF(&component1c, &component2c, &component3c, &component4c);
        break;
    default:
        break;
    }
    switch(defaultColorModel()){
    case ColorRange::cmGREYSCALE:{
        bool isGrey = component1a == component2a && component1a == component3a;
        if (!isGrey)
            return false;
    }
    case ColorRange::cmRGBA :
    case ColorRange::cmHSLA :
        return component1a >= component1b && component1a <= component1c &&
                component1a >= component2b && component1a <= component2c &&
                component1a >= component3b && component1a <= component3c;
    case ColorRange::cmCYMKA:
        return component1a >= component1b && component1a <= component1c &&
                component1a >= component2b && component1a <= component2c &&
                component1a >= component3b && component1a <= component3c &&
                component1a >= component4b && component1a <= component4c;

    }
    return false;

}

bool ContinousColorRange::contains(ColorRange *v, bool inclusive) const
{
    ContinousColorRange *ccr = dynamic_cast< ContinousColorRange*>(v);
    if ( ccr){
        return contains(IVARIANT(ccr->_limit1), inclusive) && contains(IVARIANT(ccr->_limit2),inclusive);
    } else {
        ColorPalette *pcr = dynamic_cast< ColorPalette*>(v);
        for(quint32 index = 0; index < pcr->count(); ++index) {
            if ( !contains(pcr->color(index), inclusive)){
                return false;
            }
        }
        return true;
    }
    return false;
}

QVariant ContinousColorRange::impliedValue(const QVariant &v) const
{
    QColor clr = toColor(v, defaultColorModel());
    if ( !clr.isValid())
        return QColor();
    if ( contains(clr))
        return clr;

    return QColor();

}

IlwisTypes ContinousColorRange::valueType() const
{
    return itCONTINUOUSCOLOR;
}

//---------------------------------------------------------------
QVariant ColorPalette::impliedValue(const QVariant &v) const
{
    QColor clr = ColorRange::toColor(v, defaultColorModel());
    if ( !clr.isValid())
        return QColor();
    if ( contains(clr))
        return clr;

    return QColor();
}

quint32 ColorPalette::count() const
{
    return _colors.size();
}

IlwisTypes ColorPalette::valueType() const
{
    return itPALETTECOLOR;
}

SPDomainItem ColorPalette::item(quint32 raw) const
{
    if (raw < count())
        return _colors[raw];
    return SPDomainItem();
}

SPDomainItem ColorPalette::item(const QString &nam) const
{
    //TODO not sure if there is a usefull implementation;
    return SPDomainItem();
}

SPDomainItem ColorPalette::itemByOrder(quint32 index) const
{
    return item(index);
}

QColor ColorPalette::color(int index) const
{
    if ( index < _colors.size())
        return _colors[index]->color();
    return clrUNDEF;
}

void ColorPalette::add(DomainItem *item)
{
    _colors.push_back( SPColorItem(dynamic_cast<ColorItem *>(item)));
}

void ColorPalette::add(SPDomainItem item)
{
    SPColorItem citem = item.staticCast<ColorItem>();
    _colors.push_back(citem);
}

void ColorPalette::remove(const QString &nm)
{
    ItemIterator<ColorItem> iter(this);
    std::vector<int> toBeRemoved;
    int count = 0;
    while(iter != iter.end()){
        ColorItem *item = *iter;
        if ( item->name() == nm){
            toBeRemoved.push_back(count++);
        }
    }
    for(int i=0; i < toBeRemoved.size(); ++i){
        _colors.erase(_colors.begin() + toBeRemoved[i] - i);
    }
}

void ColorPalette::clear()
{
    _colors.resize(0);
}

bool ColorPalette::contains(const QVariant &color, bool inclusive) const
{
    if (!isValid())
        return false;

    for(const SPColorItem& clr : _colors){
        if ( clr->color() == color)
            return true;
    }
    return false;
}

bool ColorPalette::contains(SPRange rng, bool inclusive) const
{
    ItemIterator<ColorItem> iter(static_cast<ItemRange *>((rng.data())));
    while(iter != iter.end()){
        ColorItem *item = *iter;
        if (!contains(item->color()))
            return false;
    }
    return true;
}

bool ColorPalette::contains(ItemRange *rng, bool inclusive) const
{
    ItemIterator<ColorItem> iter(rng);
    while(iter != iter.end()){
        ColorItem *item = *iter;
        if (!contains(item->color()))
            return false;
    }
    return true;
}

QVariant ColorPalette::ensure(const QVariant &v, bool inclusive) const
{
    if ( !contains(v, inclusive))
        return QColor();
    return v;
}

bool ColorPalette::isValid() const
{
    return count() != 0;
}





