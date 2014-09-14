#include <QColor>
#include <QRegularExpression>
#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "domainitem.h"
#include "domain.h"
#include "itemrange.h"
#include "rangeiterator.h"
#include "coloritem.h"
#include "itemiterator.h"
#include "datadefinition.h"
#include "colorrange.h"

using namespace Ilwis;

//const quint64 colormask1 = 0xF000000000000000;
//const quint64 colormask2 = 0xF00000000000;
//const quint64 colormask3 = 0xF0000000;
//const quint64 colormask4 = 0xF000;



ColorRangeBase::ColorRangeBase(IlwisTypes tp, ColorModel clrmodel) : _valuetype(tp), _defaultModel(clrmodel)
{
}

ColorRangeBase::ColorModel ColorRangeBase::defaultColorModel() const
{
    return _defaultModel;
}

void ColorRangeBase::defaultColorModel(ColorRangeBase::ColorModel m)
{
    _defaultModel = m;
}

QColor ColorRangeBase::toColor(const QVariant &v, ColorRangeBase::ColorModel colormodel)
{
    if ( v.type() == QVariant::Color)
        return QColor(v.value<QColor>());
    else if ( v.type() == QVariant::String){
        QRegExp separ("[(]|,|[)]");
        QStringList parts = (v.toString()).split(separ);
        if(parts.last().isEmpty())
            parts.removeLast();
        QColor clr;
        bool ok1,ok2,ok3,ok4,ok5 =true;
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
            }else if ( parts[0].toLower() == "hsla"){
                if ( isFractional){
                   clr.setHslF(component1,component2, component3);
                   clr.setAlphaF(component4);
                }
                else{
                    clr.setHsl(component1,component2, component3);
                    clr.setAlpha(component4);
                }

            } else if ( parts[0].toLower() == "cmyka" && parts.size() == 6){
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
    } else if( v.type() == QVariant::ULongLong){
        return ColorRangeBase::toColor(v.toULongLong(),colormodel);
    } else if( v.type() == QVariant::Double){
        return ColorRangeBase::toColor(v.toULongLong(),colormodel);
    }

    return QColor();
}

QString ColorRangeBase::toString(const QColor &clr, ColorRangeBase::ColorModel clrModel)
{
    QString color;
    switch(clrModel){
    case ColorRangeBase::cmRGBA:
        color += QString("RGBA(%1 %2 %3 %4)").arg(clr.redF()).arg(clr.greenF()).arg(clr.blueF()).arg(clr.alphaF());
        break;
    case ColorRangeBase::cmHSLA:
        color += QString("HSLA(%1 %2 %3 %4)").arg(clr.hueF()).arg(clr.saturationF()).arg(clr.lightnessF()).arg(clr.alphaF());
        break;
    case ColorRangeBase::cmCYMKA:
        color += QString("CMYKA(%1 %2 %3 %4 %5)").arg(clr.cyanF()).arg(clr.magentaF()).arg(clr.yellowF()).arg(clr.blackF()).arg(clr.alphaF());
        break;
    case ColorRangeBase::cmGREYSCALE:
        color += QString("GREY(%1)").arg(clr.red());
        break;
    default:
        break;
    }
    return color;
}

void ColorRangeBase::storeColor(const QColor& clr, QDataStream &stream)
{
    switch (defaultColorModel()){
    case ColorRangeBase::cmRGBA:
        stream << clr.red() << clr.green() << clr.blue() << clr.alpha();
        break;
    case ColorRangeBase::cmHSLA:
        stream << clr.hue() << clr.saturation() << clr.lightness() << clr.alpha();
        break;
    case ColorRangeBase::cmCYMKA:
        stream << clr.yellow() << clr.magenta() << clr.cyan() << clr.black();
        break;
    case ColorRangeBase::cmGREYSCALE:
        stream << clr.red();
    default:
        break;
    }
}

void ColorRangeBase::loadColor(QColor& clr, QDataStream &stream){
    int c1, c2, c3, c4;

    stream >> c1 >> c2 >> c3 >> c4;

    switch (defaultColorModel()){
    case ColorRangeBase::cmRGBA:
        clr.setRgb(c1,c2,c3);
        clr.setAlpha(c4);
        break;
    case ColorRangeBase::cmHSLA:
        clr.setHsl(c1,c2,c3);
        clr.setAlpha(c4);
        break;
    case ColorRangeBase::cmCYMKA:
        clr.setCmyk(c1,c2,c3, c4);
        break;
    case ColorRangeBase::cmGREYSCALE:
        stream << clr.red();
    default:
        break;
    }
}

QColor ColorRangeBase::toColor(quint64 clrint, ColorModel clrModel)
{

    LocalColor *localcolor = reinterpret_cast<LocalColor *>(&clrint);

    QColor clr;
    if ( clrModel == ColorRangeBase::cmRGBA){
        clr = QColor(localcolor->_component1,localcolor->_component2, localcolor->_component3);
        clr.setAlpha(localcolor->_component4);
    }
    else if ( clrModel ==  ColorRangeBase::cmHSLA){
        clr.setHsl(localcolor->_component1,localcolor->_component2, localcolor->_component3);
        clr.setAlpha(localcolor->_component4);
    } else if ( clrModel ==  ColorRangeBase::cmCYMKA){
        clr.setCmyk(localcolor->_component1,localcolor->_component2, localcolor->_component3, localcolor->_component4);
        //clr.setAlpha(localcolor->_component4); ??
    }
    return clr;
}

//----------------------------------------------------------------
ContinousColorRange::ContinousColorRange() : ColorRangeBase(itCONTINUOUSCOLOR,ColorRangeBase::cmRGBA)
{

}

ContinousColorRange::ContinousColorRange(const QColor &clr1, const QColor &clr2, ColorRangeBase::ColorModel colormodel) : ColorRangeBase(itCONTINUOUSCOLOR,colormodel), _limit1(clr1), _limit2(clr2)
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

    return ColorRangeBase::toString(_limit1, defaultColorModel()) + "," + ColorRangeBase::toString(_limit2, defaultColorModel());
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
    QColor clr;
    if( v.type() == QVariant::Double){
        clr =  ColorRangeBase::toColor(v.toULongLong(),defaultColorModel());
    } else
        clr = v.value<QColor>();
    if ( !clr.isValid())
        return false;

    double component1a, component2a, component3a,component4a;
    double component1b, component2b, component3b,component4b;
    double component1c, component2c, component3c,component4c;
    QColor limit1,limit2;

    switch (defaultColorModel()) {
    case ColorRangeBase::cmRGBA :
    case ColorRangeBase::cmGREYSCALE:
        clr.getRgbF(&component1a, &component2a, &component3a);
        _limit1.getRgbF(&component1b, &component2b, &component3b);
        _limit2.getRgbF(&component1c, &component2c, &component3c);
        break;
    case ColorRangeBase::cmHSLA :
        clr.getHslF(&component1a, &component2a, &component3a);
        _limit1.getHslF(&component1b, &component2b, &component3b);
        _limit2.getHslF(&component1c, &component2c, &component3c);
        break;
    case ColorRangeBase::cmCYMKA:
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
    case ColorRangeBase::cmGREYSCALE:{
        bool isGrey = component1a == component2a && component1a == component3a;
        if (!isGrey)
            return false;
    }
    case ColorRangeBase::cmRGBA :
    case ColorRangeBase::cmHSLA :
        return component1a >= component1b && component1a <= component1c &&
                component2a >= component2b && component2a <= component2c &&
                component3a >= component3b && component3a <= component3c;
    case ColorRangeBase::cmCYMKA:
        return component1a >= component1b && component1a <= component1c &&
                component2a >= component2b && component2a <= component2c &&
                component3a >= component3b && component3a <= component3c &&
                component4a >= component4b && component4a <= component4c;
    default:
        break;

    }
    return false;

}

bool ContinousColorRange::contains(const Range *rng,bool inclusive) const
{
    const ContinousColorRange *ccr = dynamic_cast< const ContinousColorRange*>(rng);
    if ( ccr){
        return contains(IVARIANT(ccr->_limit1), inclusive) && contains(IVARIANT(ccr->_limit2),inclusive);
    } else {
        const ColorPalette *pcr = dynamic_cast< const ColorPalette*>(rng);
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
    if ( v == clrUNDEF2)
        return QColor();

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

QColor ContinousColorRange::valueAt(quint32& index, const Range *rng)
{
    // TODO : indexes over the range of all possible colors
    return QColor();

}

QString ContinousColorRange::valueAsString(quint32& index, const Range *rng)
{
    // TODO : indexes over the range of all possible colors
    return sUNDEF;

}

quint32 ContinousColorRange::count() const
{
    return iUNDEF;
}

void ContinousColorRange::add(const QVariant &v)
{
    if ( contains(v))
        return;
    QColor clr = toColor(v, defaultColorModel());
    if ( !clr.isValid())
        return;
    if ( defaultColorModel() == ColorRangeBase::cmRGBA){
        _limit1.setRed(std::min(_limit1.red(), clr.red()));
        _limit1.setGreen(std::min(_limit1.green(), clr.green()));
        _limit1.setBlue(std::min(_limit1.blue(), clr.blue()));
        _limit1.setAlpha(std::min(_limit1.alpha(), clr.alpha()));
        _limit2.setRed(std::max(_limit2.red(), clr.red()));
        _limit2.setGreen(std::max(_limit2.green(), clr.green()));
        _limit2.setBlue(std::max(_limit2.blue(), clr.blue()));
        _limit2.setAlpha(std::max(_limit2.alpha(), clr.alpha()));
    }else if (defaultColorModel() == ColorRangeBase::cmHSLA) {
        _limit1.setHsl(std::min(_limit1.hue(), clr.hue()),
                       std::min(_limit1.saturation(), clr.saturation()),
                       std::min(_limit1.lightness(), clr.lightness()));
        _limit1.setAlpha(std::min(_limit1.alpha(), clr.alpha()));
        _limit2.setHsl(std::max(_limit2.hue(), clr.hue()),
                       std::max(_limit2.saturation(), clr.saturation()),
                       std::max(_limit2.lightness(), clr.lightness()));
        _limit2.setAlpha(std::max(_limit2.alpha(), clr.alpha()));
    }
    else if ( defaultColorModel() == ColorRangeBase::cmCYMKA){
        _limit1.setCmyk(std::min(_limit1.cyan(), clr.cyan()),
                        std::min(_limit1.magenta(), clr.magenta()),
                        std::min(_limit1.yellow(), clr.yellow()),
                        std::min(_limit1.black(), clr.black()));
        _limit1.setAlpha(std::min(_limit1.alpha(), clr.alpha()));
        _limit2.setCmyk(std::max(_limit2.cyan(), clr.cyan()),
                        std::max(_limit2.magenta(), clr.magenta()),
                        std::max(_limit2.yellow(), clr.yellow()),
                        std::max(_limit2.black(), clr.black()));
        _limit2.setAlpha(std::max(_limit2.alpha(), clr.alpha()));

    }

}

void ContinousColorRange::store(QDataStream &stream)
{
    stream << defaultColorModel();
    storeColor(_limit1, stream);
    storeColor(_limit2, stream);
}

void ContinousColorRange::load(QDataStream &stream)
{
    int model;
    stream >> model;
    defaultColorModel((ColorModel) model);
    loadColor(_limit1, stream);
    loadColor(_limit2, stream);
}

//---------------------------------------------------------------
ColorPalette::ColorPalette() : ColorRangeBase(itPALETTECOLOR,ColorRangeBase::cmRGBA)
{

}

QVariant ColorPalette::impliedValue(const QVariant &v) const
{
    QColor clr = ColorRangeBase::toColor(v, defaultColorModel());
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

QColor ColorPalette::valueAt(quint32 index, const Ilwis::Range *rng)
{
    const ColorPalette *palette = reinterpret_cast<const ColorPalette*>(rng);
    if ( index < palette->_colors.size())
        return palette->_colors.at(index)->color() ;
    index = iUNDEF;
    return QColor();
}

QString ColorPalette::valueAsString(quint32 index, const Range *rng)
{
    QColor color = valueAt(index, rng);
    return ColorRangeBase::toString(color, cmRGBA);
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

QString ColorPalette::toString() const
{
    QString result;
    for(int index = 0; index < _colors.size();++index ){
        QString itemString = ColorRangeBase::toString(_colors[index]->color(), defaultColorModel());
        if ( result != "")
            result += ",";
        result += QString::number(index) + "=" + itemString;

    }
    return result;
}

qint32 ColorPalette::gotoIndex(qint32 index, qint32 step) const
{
    if ( index + step >= _colors.size())
        return _colors.size();
    if ( index + step < 0)
        return 0;
    return index + step;
}
Range *ColorPalette::clone() const
{
    ColorPalette *palette = new ColorPalette();
    for(const auto& item : _colors){
        palette->add( item->clone());
    }
    palette->defaultColorModel(defaultColorModel());

    return palette;
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

void ColorPalette::add(const QVariant &color)
{
    if (contains(color))
        return;
    QColor newColor = toColor(color, defaultColorModel());
    add(new ColorItem(newColor));
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

bool ColorPalette::contains(Range *rng, bool inclusive) const
{
    if ( !rng || rng->valueType() != itPALETTECOLOR)
        return false;

    ColorPalette *palette = static_cast<ColorPalette *>(rng);
    if (!palette)
        return false;
    RangeIterator<QColor, ColorPalette> iter(rng);
    while(iter.isValid()){
        QColor clr = *iter;
        if (!contains(clr))
            return false;
        ++iter;
    }
    return true;
}

QVariant ColorPalette::ensure(const QVariant &v, bool inclusive) const
{
    if ( hasType(Domain::ilwType(v), itNUMBER) ){
        bool ok;
        int index = v.toInt(&ok);
        if ( !ok || index < 0 || _colors.size() <= index)
            return QVariant();
        return v;
    }
    if ( !contains(v, inclusive))
        return QColor();
    return v;
}

bool ColorPalette::isValid() const
{
    return count() != 0;
}

bool ColorPalette::alignWithParent(const IDomain &dom){
    //TODO dont think the internal structure of a palette is conductive to parent/child relations at the moment
    return false;
}

void ColorPalette::store(QDataStream &stream)
{
    stream << _colors.size();
    for(const auto& clrItem : _colors)    {
        storeColor(clrItem->color(), stream);
    }
}

void ColorPalette::load(QDataStream &stream)
{
    int numberOfColor;
    stream >> numberOfColor;
    for(int i = 0; i < numberOfColor; ++i)    {
        QColor clr;
        loadColor(clr, stream);
        add(new ColorItem(clr));
    }
}




