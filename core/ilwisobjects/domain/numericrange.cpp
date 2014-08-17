#include <QDataStream>
#include "kernel.h"
#include "numericrange.h"

using namespace Ilwis;

NumericRange::NumericRange() : _min(0), _max(-1),_resolution(0) {

}
NumericRange::NumericRange(double mi, double ma, double step) : _resolution(step), _undefined(rUNDEF) {
    min(mi);
    max(ma);
}
NumericRange::NumericRange(const NumericRange &vr): _undefined(rUNDEF)
{
    set(vr);
}

bool NumericRange::isValid() const
{
    return _min <= _max && _resolution >= 0;
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
    } else
        _min = v;
}

double NumericRange::distance() const
{
    if ( !isValid())
        return rUNDEF;

    return std::abs(_max - _min);
}

void NumericRange::add(const QVariant &number)
{
    if ( contains(number))
        return;
    bool ok;
    double value = number.toDouble(&ok);
    if (!ok || isNumericalUndef(value))
        return;
    _min = std::min(value, _min);
    _max = std::max(value, _max);
}

void NumericRange::max(double v)
{
    if (_resolution == 1){
        _max = (qint64) v;
    } else
        _max = v;
}

void NumericRange::resolution(double step) {
    if ( step >= 0)
        _resolution = step;
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

QString NumericRange::toString() const {
    if(!isValid())
        return "? ? ?";
    long n1 = significantDigits(_min);
    long n2 = significantDigits(_max);
    long n = std::max(n1, n2);
    bool isfloat = _resolution == 0 || _resolution - (quint64)_resolution != 0;
    if (isfloat){
        QString rng = QString::number(_min,'f',n);
        rng += ' ';
        rng += QString::number(_max,'f',n);
        if ( _resolution != 0){
            rng += ' ';
            rng += QString::number(_resolution) ;
        }
        return rng;
    }
    QString rng = QString("%1 %2 %3").arg(_min).arg(_max).arg(_resolution);
    return rng;
}

QVariant NumericRange::impliedValue(const QVariant &v) const
{
    bool ok;
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

    QString s = QString::number(m1);
    for(int i=s.size() - 1; i != 0; --i ) {
        QChar c = s[i];
        if ( c != '0') {
            if ( s.indexOf(".") > 0) // '.' is not counted for significant numbers
                return i;
            return i -1;
        }
    }
    return s.size();
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

}

quint32 NumericRange::count() const
{
    if ( _resolution == 0 || !isValid())
        return iUNDEF;

    return 1 + distance() / _resolution;
}

void NumericRange::store(QDataStream &stream)
{
    _valuetype = itUNKNOWN;
    stream << _min << _max << _resolution << _undefined;
}

void NumericRange::load(QDataStream &stream)
{
    _valuetype = itUNKNOWN;
    stream >> _min >> _max >> _resolution >> _undefined;
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

