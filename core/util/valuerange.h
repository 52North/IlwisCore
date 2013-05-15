#ifndef VALUERANGE_H
#define VALUERANGE_H

#include <QString>
#include <typeinfo>
#include "kernel.h"
#include "range.h"

namespace Ilwis {
template<class T=double> class ValueRange : public Range
{
public:
    ValueRange() : _min(0), _max(-1) {}
    ValueRange(T mi, T ma) : _min(mi), _max(ma) {}
    ValueRange(const ValueRange<T> &vr)
    {
        _min = vr.min();
        _max = vr.max();
    }

    bool isValid() const
    {
        return _min <= _max ;
    }

    bool contains(T v, bool inclusive = true) const
    {
        if (!isValid())
            return false;
        if ( inclusive)
            return v >= _min && v <= _max;
        return v > _min && v < _max;
    }
    T max() const
    {
        return _max;
    }
    T min() const
    {
        return _min;
    }
    void setMin(T v)
    {
        _min = v;
    }

    void setMax(T v)
    {
        _max = v;
    }

    ValueRange& operator+=(T v)
    {
        if ( !isValid())
            _min = _max = v;
        else {
            if ( v > _max )
                _max = v;
            if ( v < _min)
             _min = v;
        }
        return *this;
    }

    bool operator==(const ValueRange& vr) {
        return vr.max() == max() && vr.min() == min();
    }

    QString toString() const { return "";}
    void set(const ValueRange<T>& vr)
    {
        _min = vr.min();
        _max = vr.max();
    }


    IlwisTypes findBaseType() const {
        QString t = kernel()->demangle(typeid(T).name());
        bool intType = t == "int" || t == "char" || t == "long" || t == "long long" || t == "short" ||
                t == "unsigned int" || t == "unsigned char" || t == "unsigned long" || t == "unsigned short";
        return intType ? itINT64 : itDOUBLE;
    }

    IlwisTypes determineType() const{
        IlwisTypes vt = findBaseType();
        if ( vt == itINT64) { // integer part
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
        return vt;
    }

private:
    T _min;
    T _max;

    long significantDigits(double m1) const{
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


};
}

#endif // VALUERANGE_H
