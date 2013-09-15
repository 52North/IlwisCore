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
    NumericRange(double mi, double ma, double step=0);
    /*!
     * \brief NumericRange copy constructor
     * \param vr other value range
     */
    NumericRange(const NumericRange &vr);


    bool isValid() const;
    Range *clone() const ;

    bool contains(double v, bool inclusive = true) const;
    bool contains(const QString& value, bool inclusive = true) const;
    bool contains(SPRange rng, bool inclusive=true) const;
    double max() const;
    void setMax(double v);
    double min() const;
    void setMin(double v);
    double distance() const;

    void setStep(double step);
    double step() const ;

    NumericRange& operator+=(double v);

    bool operator==(const NumericRange& vr);
    bool operator<(const NumericRange& vr);
    bool operator>(const NumericRange& vr);

    QString toString() const ;
    void set(const NumericRange& vr);
    double ensure(double, bool inclusive=true) const;
    IlwisTypes determineType() const;

    static NumericRange *merge(const QSharedPointer<NumericRange>& nr1, const QSharedPointer<NumericRange>& nr2);

private:
    double _min;
    double _max;
    double _step;
    double _undefined;

    long significantDigits(double m1) const;
};

typedef QSharedPointer<NumericRange> SPNumericRange;

}

#endif // NUMERICVALUERANGE_H
