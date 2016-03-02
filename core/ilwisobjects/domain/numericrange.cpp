#include <QDataStream>
#include "kernel.h"
#include "numericrange.h"

using namespace Ilwis;

NumericRange::NumericRange() : _min(1e300), _max(-1), _resolution(0) {

}

NumericRange::NumericRange(double mi, double ma, double step) : _min(mi), _max(ma), _resolution(step), _undefined(rUNDEF) {
    _valuetype = determineType(true);
}

NumericRange::NumericRange(const NumericRange &vr): _undefined(rUNDEF)
{
    set(vr);
}

NumericRange::NumericRange(const QString& defintion){
    QStringList parts = defintion.split(":");
    if ( parts.size() == 2 && parts[0] == "numericrange"){
        QStringList numberParts = parts[1].split("|");
        if ( numberParts.size() == 2 || numberParts.size() == 3){
            bool ok1, ok2, ok3;
            _min = numberParts[0].toDouble(&ok1);
            _max = numberParts[1].toDouble(&ok2);
            if ( numberParts.size() == 3){
                _resolution = numberParts[2].toDouble(&ok3);
            }
            if ( !ok1 || !ok2 || !ok3){
                _min = 1e300;
                _max = -1;
            }
        }
    }
}

bool NumericRange::isValid() const
{
    return _min <= _max && _resolution >= 0 && !isNumericalUndef(_min) && !isNumericalUndef(_max);
}

Range *NumericRange::clone() const {
    NumericRange *rng = new NumericRange(_min, _max, _resolution);
    rng->interpolation(interpolation());
    return rng;
}

double NumericRange::max() const
{
    return _max;
}
double NumericRange::min() const
{
    return _min;
}
void NumericRange::min(double v)
{
    if (_resolution == 1){
        _min = (qint64) v;
    } else{
        _min = _resolution==0 ? v : _resolution * qint64(v / _resolution);
    }
}

double NumericRange::distance() const
{
    if ( !isValid())
        return rUNDEF;

    return std::abs(_max - _min);
}

double NumericRange::center() const
{
    if ( !isValid())
        return rUNDEF;

    return (_min + _max) / 2.0;
}

void NumericRange::add(const QVariant &number)
{
    bool ok;
    double value = number.toDouble(&ok);
    if (!ok || isNumericalUndef(value))
        return;
    min(std::min(value, _min));
    max(std::max(value, _max));
}

void NumericRange::max(double v)
{
    if (_resolution == 1){
        _max = (qint64) v;
    } else
        _max = _resolution==0 ? v : _resolution * qint64(v / _resolution);
}

void NumericRange::resolution(double step) {
    if ( step >= 0){
        _resolution = step;
        min(_min);
        max(_max);
    }
}

double NumericRange::resolution() const {
    return _resolution;
}

NumericRange& NumericRange::operator+=(double v)
{
    if ( !isValid()){
        min(v);
        max(v);
    }
    else {
        if ( v > _max )
            max(v);
        if ( v < _min)
            min(v);
    }
    return *this;
}

void NumericRange::add(double v)
{
    if ( isNumericalUndef(v))
        return;

    if ( v < min())
         min(v);
    if ( v > max())
        max(v);
}

bool NumericRange::operator==(const NumericRange& vr) {
    return vr.max() == max() && vr.min() == min() && vr.resolution() == resolution();
}

bool NumericRange::operator<(const NumericRange &vr)
{
    return max() < vr.max() && min() < vr.min() && max() < vr.min();
}

bool NumericRange::operator>(const NumericRange &vr)
{
    return max() > vr.max() && min() > vr.min() && min() > vr.max();
}

bool NumericRange::operator>=(const NumericRange &vr)
{
    return max() >= vr.max() && min() >= vr.min() && min() >= vr.max();
}

bool NumericRange::operator<=(const NumericRange &vr)
{
    return max() <= vr.max() && min() <= vr.min() && max() <= vr.min();
}


QString NumericRange::toString() const {
    if(!isValid())
        return "? ? ?";
    int n1 = significantDigits(_min);
    int n2 = significantDigits(_max);
    int n = std::max(n1, n2);
    bool isfloat = _resolution == 0 || _resolution - (quint64)_resolution != 0;
    if (isfloat){

        int ln1 = std::log10(std::abs(_min));
        int ln2 = std::log10(std::abs(_max));
        int ln = std::max(ln1, ln2);
        ln = ln >= 0 ? ln : 1;
        n = std::max(0,std::min(6 - ln, n));

        QString rng = (_min < -1e100 || std::abs(_min) == 4294967295) ? "-infinite" : QString::number(_min,'f',n);

        rng += '|';
        rng += (_max > 1e100 || std::abs(_max) == 4294967295) ? "+infinite": QString::number(_max,'f',n);
        if ( _resolution != 0){
            rng += '|';
            rng += QString::number(_resolution) ;
        }
        return "numericrange:" + rng;
    }
    QString rng = QString("%1|%2|%3").arg(_min).arg(_max).arg(_resolution);
    return "numericrange:"+ rng;
}

QVariant NumericRange::impliedValue(const QVariant &v) const
{
    bool ok;
    if(!v.isValid())
        return _undefined;

    if ( v == sUNDEF)
        return _undefined;

    double vtemp = v.toDouble(&ok);
    if (!ok){
        ERROR2(ERR_COULD_NOT_CONVERT_2,v.toString(), "number");
        return sUNDEF;
    }
    if ( _resolution == 0) // everything goes
        return v;
    vtemp = ensure(vtemp).value<double>();
    if ( vtemp == _undefined)
        return vtemp;

    if ( (std::floor(_resolution) == _resolution) ) {
        if (_min >= 0)
            return (quint64)vtemp;
        else
            return (qint64)vtemp;
    }
    return vtemp;

}

IlwisTypes NumericRange::valueType() const
{
    if (_valuetype == itUNKNOWN)
        const_cast<NumericRange *>(this)->determineType();
    return _valuetype;
}
void NumericRange::set(const NumericRange& vr)
{
    _resolution = vr._resolution;
    min(vr._min);
    max(vr._max);
    _valuetype = determineType(true);
}

bool NumericRange::contains(const QVariant &value, bool inclusive) const
{
    bool ok;
    double v = value.toDouble(&ok);
    if(QString(value.typeName()).compare("Ilwis::Time") == 0){
        Time temp = value.value<Ilwis::Time>();
        QVariant qVar ((double)temp);
        v = qVar.toDouble(&ok);
    }
    if (!ok)
        return false;
    if ( v == rUNDEF)
        return false;
    return contains(v, inclusive);
}

bool NumericRange::contains(SPRange rng, bool inclusive) const
{
    if ( rng.isNull())
        return false;
    SPNumericRange numrange = rng.staticCast<NumericRange>();
    bool ok = contains(numrange->min(), inclusive);
    return ok && contains(numrange->max(), inclusive);
}

bool NumericRange::contains(NumericRange *rng, bool inclusive) const
{
    if ( !rng)
        return false;
    bool ok = contains(rng->min(), inclusive);
    return ok && contains(rng->max(), inclusive);
}


long NumericRange::significantDigits(double m1) const{
    if ( fabs(m1) > 1e30)
        return 100;
   QString s = QString::number(m1,'f',6);
    int index = s.indexOf('.');
    if ( index == -1)
        return 0;

    int mod = index != -1 ? index : 0;
    bool tail9 = true;
    for(int i=s.size() - 1; i != 0; --i ) {
        QChar c = s[i];
        if ( c != '0') {
            return i +1 - mod;
        }
        if ( c == '9' && tail9){
            ++mod;
        }else
            tail9 = false;

    }
    return s.size() - mod;
}

IlwisTypes NumericRange::determineType(bool redo) {

    if ( _valuetype == itUNKNOWN || redo){
        IlwisTypes vt = itUNKNOWN;
        if ( _resolution == 1) { // integer part
            bool sig = min() < 0;
            if ( max() <=128 && sig)
                vt =  itINT8;
            else if ( max() <= 255 && !sig)
                vt =  itUINT8;
            else if ( max() <= 32768 && sig)
                vt =  itINT16;
            else if ( max() <= 65536 && !sig)
                vt =  itUINT32;
            else if ( max() <= 2147483647 && sig)
                vt =  itINT32;
            else if ( max() <= 4294967296 && !sig)
               vt =  itUINT32;
            else
                vt =  itINT64; // unlikely but anyway
        } else { // real part
            int signif1 = std::max(significantDigits(min()), significantDigits(max()));
            if ( signif1 > 6)
               vt =  itDOUBLE;
            else
                vt =  itFLOAT;
        }
        _valuetype = vt;
    }
    return _valuetype;
}

void NumericRange::clear()
{
    _min = 0;
    _max = -1;
    _undefined = rUNDEF;
    _valuetype = itUNKNOWN;

}

quint32 NumericRange::count() const
{
    if ( _resolution == 0 || !isValid())
        return iUNDEF;

    return 1 + distance() / _resolution;
}

void NumericRange::store(QDataStream &stream)
{
    stream << _min << _max << _resolution << _undefined << _valuetype;
}

void NumericRange::load(QDataStream &stream)
{
    stream >> _min >> _max >> _resolution >> _undefined >> _valuetype;
}

NumericRange *NumericRange::merge(const QSharedPointer<NumericRange> &nr1, const QSharedPointer<NumericRange> &nr2, RenumberMap *rnm)
{
    return new NumericRange(std::min(nr1->min(), nr2->min()),
                            std::max(nr1->max(), nr2->max()),
                            std::min(nr1->resolution(), nr2->resolution()));
}
double NumericRange::valueAt(quint32& index, const Range *rng)
{
    if ( rng && hasType(rng->valueType(), itNUMBER) ){
        const NumericRange *numrange = static_cast<const NumericRange *>(rng);
        if ( numrange->resolution() != 0 ) {
            double v = numrange->min() + numrange->resolution() * index;
            if ( v > numrange->max())
                v = rUNDEF;
            return v;
        }
    }
    index = iUNDEF;
    return rUNDEF;
}

QString NumericRange::valueAsString(quint32 &index, const Range *rng)
{
    double value = valueAt(index, rng);
    return QString::number(value) ;
}

