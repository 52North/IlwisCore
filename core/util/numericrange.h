#ifndef NUMERICVALUERANGE_H
#define NUMERICVALUERANGE_H

#include <QString>
#include "range.h"

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


    bool isValid() const;
    Range *clone() const ;

    bool contains(double v, bool inclusive = true) const{
        if (!isValid())
            return false;

        if ( inclusive)
            return v >= _min && v <= _max;
        return v > _min && v < _max;
    }


    bool contains(const QVariant &value, bool inclusive = true) const;
    bool contains(SPRange rng, bool inclusive=true) const;
    bool contains(NumericRange *rng, bool inclusive=true) const;
    double max() const;
    void max(double v);
    double min() const;
    void min(double v);
    double distance() const;

    void resolution(double resolution);
    double resolution() const ;

    NumericRange& operator+=(double v);

    bool operator==(const NumericRange& vr);
    bool operator<(const NumericRange& vr);
    bool operator>(const NumericRange& vr);

    QString toString() const ;
    QVariant impliedValue(const QVariant& v) const;
    void set(const NumericRange& vr);
    double ensure(double v, bool inclusive=true) const
    {
        if ( !contains(v, inclusive))
            return _undefined;

        if ( _resolution != 0.0)
            v = (qint64)(v / _resolution) * _resolution;
        return v;
    }
    IlwisTypes determineType() const;
    void clear();

    static NumericRange *merge(const QSharedPointer<NumericRange>& nr1, const QSharedPointer<NumericRange>& nr2,RenumberMap *rnm=0);

private:
    double _min;
    double _max;
    double _resolution;
    double _undefined;

    long significantDigits(double m1) const;
};

typedef QSharedPointer<NumericRange> SPNumericRange;

}

#endif // NUMERICVALUERANGE_H
