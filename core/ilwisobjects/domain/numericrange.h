#ifndef NUMERICVALUERANGE_H
#define NUMERICVALUERANGE_H

#include <QString>
#include "range.h"
#include "rangeiterator.h"

namespace Ilwis {

/*!
 * \brief The NumericRange class describes a range of numbers between a minimum and maximum combined with a resolution
 */
class KERNELSHARED_EXPORT NumericRange : public Range
{
public:
    NumericRange();
    /*!
     * \brief NumericRange basic constructor for the range with a minimum and a maximum and a resolution.
     *
     *If the resolution is 0, the range has same resolution a double. A step size of 0 will make the range iterator invalid. This prefectly legal, it only makes the iterator not usable.
     *A resolution of an integer will make the range an integer range
     *
     * \param mi minimum of the range
     * \param ma maximum of the range
     * \param step resolution of the range.
     */
    NumericRange(double mi, double ma, double resolution=0);
    /*!
     * \brief NumericRange copy constructor
     * \param vr other value range
     */
    NumericRange(const NumericRange &vr);
    NumericRange(const QString& def);


    bool isValid() const;
    Range *clone() const ;

    bool contains(double v, bool inclusive = true) const{
        if (!isValid())
            return false;
        if ( isNumericalUndef(v))
            return false;

        bool ok = inclusive ? v >= _min && v <= _max : v > _min && v < _max;
        if (!ok || _resolution < 0.01 || _resolution == 1)
            return ok;
        double floattest = (v - _min) / _resolution;
        double inttest = round(floattest);
        double delta = fabs(floattest - inttest);
        return delta < EPS7; // EPS10 is too much to ask; the computed values aren't that accurate
    }


    bool contains(const QVariant &value, bool inclusive = true) const;
    bool contains(SPRange rng, bool inclusive=true) const;
    bool contains(NumericRange *rng, bool inclusive=true) const;
    double max() const;
    void max(double v);
    double min() const;
    void min(double v);
    double distance() const;
    double center() const;
    void add(const QVariant& number);

    void resolution(double resolution);
    double resolution() const ;

    NumericRange& operator+=(double v);
    void add(double v);

    bool operator==(const NumericRange& vr);
    bool operator<(const NumericRange& vr);
    bool operator>(const NumericRange& vr);
    bool operator >=(const NumericRange &vr);
    bool operator <=(const NumericRange &vr);

    QString toString() const ;
    QVariant impliedValue(const QVariant& v) const;
    IlwisTypes valueType() const;
    IlwisTypes determineType(bool redo=false) ;
    void set(const NumericRange& vr);
    QVariant ensure(const QVariant& v, bool inclusive=true) const
    {
        double value = v.toDouble();
        if ( _resolution != 0.0)
             value = (double)((qint64)(value / _resolution)) * _resolution;
        if ( !contains(value, inclusive))
            return _undefined;

        return value;
    }

    void clear();
    quint32 count() const;
    void store(QDataStream& stream);
    void load(QDataStream& stream);

    static NumericRange *merge(const QSharedPointer<NumericRange>& nr1, const QSharedPointer<NumericRange>& nr2,RenumberMap *rnm=0);

    static double valueAt(quint32& index, const Range *rng);

    static QString valueAsString(quint32 &index, const Range *rng);
private:

    double _min;
    double _max;
    double _resolution;
    double _undefined;
    IlwisTypes _valuetype = itUNKNOWN;

    long significantDigits(double m1) const;

};

typedef QSharedPointer<NumericRange> SPNumericRange;
typedef Ilwis::RangeIterator<double, Ilwis::NumericRange> NumericRangeIterator;

inline NumericRangeIterator begin(const Ilwis::NumericRange& rng) {
    return NumericRangeIterator(&rng);
}

inline NumericRangeIterator end(const Ilwis::NumericRange& rng) {
    auto iter = NumericRangeIterator(&rng);
    iter += rng.count();
    return iter;
}


}

Q_DECLARE_METATYPE(Ilwis::NumericRange)
Q_DECLARE_METATYPE(Ilwis::SPNumericRange)





#endif // NUMERICVALUERANGE_H
