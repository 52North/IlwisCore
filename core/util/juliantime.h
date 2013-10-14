#ifndef TIME_H
#define TIME_H

#include "Kernel_global.h"

namespace Ilwis {
/*!
 * \brief The Time class  utility class that handles all time related things within Ilwis
 *
 *The time class is build upon the notion of the Julianday (astronomical) which starts its counting at 1 januari, noon, -4713 BC.
 *In Ilwis the julian day is expressed as a double so fractional days are allowed. This enables the expression of hours, minutes and seconds. As far as Ilwis is considered the a time object is just a number so normal mathematical operations apply to it.
 *To the outside world the time class is often represented as an ISO 8601 compatible string and this is also one of the notation is expects as input.
 *
 */
class KERNELSHARED_EXPORT Time {
public:
    enum TimePart{tpYEAR, tpMONTH, tpDAYOFMONTH, tpJULIANDAY, tpDAYOFTHEWEEK, tpHOUR, tpMINUTE, tpSECOND, tpDATE, tpDAYTIME};
    enum Mode{mUNKNOWN, mDATE,mDATETIME,mTIME,mDURATION};
    Time(int yr=-4713, int mnth=1, int dy=1, int hr=0, int min=0, double sec=0);
    Time(const time_t time);
    Time(const QString& isostring);
    Time(double secondssince0);
    Time(const QDateTime &time);
    virtual ~Time();
    operator double() const;
    operator QDateTime() const;
    Time& operator=(double t);
    time_t toTime_t() const;
    bool isLeapYear(int year = iUNDEF) const;
    virtual double get(TimePart part) const;
    Time operator-(const Time& time) const;
    Time operator+(const Time& time) const;
    bool operator <(const Time& time) const;
    bool operator >(const Time& time) const;
    bool operator ==(const Time& time) const;
    bool operator !=(const Time& time) const;
    void setValue(const QString& isostring);
    void setYear(int year);
    void setMonth(int Mnt);
    void setDay(int dy);
    void setHour(int hr) ;
    void setMinute(int min);
    void setSecond(double sec);
    virtual QString toString(bool local= true, Time::Mode mode=Time::mDATETIME) const;
    virtual bool isValid() const;
    static Time now();

protected:
    double gregorianToJulian(int year, int month, int day, int hour, int minutes, double seconds) const;
    void julianToGregorian(int& year, int& month, int& day, int& hour, int& minutes, double& seconds) const;
    int julianyearday(int year, int month, int day) const;
    void monthday(int year, int jday, int& month, int& day) const;
    int tolong(double val) const;
    void parseOrdinalDate(int ordinal, int year, int& month, int& day);
    void parseYearPart(const QString& yearpart, int& year, int& month, int& day) ;
    void parseDayPart(const QString& dayPart, int& hours, int& minutes, double& seconds);
    void parseIsoString(const QString& isostring, int& year, int& month, int& day, int& hours, int& minutes, double& seconds);

    double _julianday;
    bool _valid;
};

class KERNELSHARED_EXPORT Duration : public Time {
public:
    Duration(const QString& step="");
    Duration(double r);
    QString toString(bool local, Time::Mode mode) const;
    bool isValid() const;
    double get(TimePart part) const;
};

class KERNELSHARED_EXPORT TimeInterval : public NumericRange {
public:
    TimeInterval();
    TimeInterval(const Time& begin, const Time& end, const Duration& stp=Duration(""));
    TimeInterval operator+(const TimeInterval& interval);
    TimeInterval operator-(const TimeInterval& interval);
    TimeInterval& operator=(const TimeInterval& tiv);
    Time begin() const { return Time(min());}
    Time end() const { return Time(max());}
    void begin(const Time& t) ;
    void end(const Time& t);
    Duration getStep() const { return _step;}
    QString toString(bool local=true, Time::Mode mode=Time::mDATETIME);
    bool contains(const QString& value, bool inclusive = true) const;
    bool contains(const Time& value, bool inclusive = true) const;
    QString value(const QVariant& v) const;

private:
    Duration _step;

};

#define tUNDEF Time(0)

}

Q_DECLARE_METATYPE(Ilwis::Time)
Q_DECLARE_METATYPE(Ilwis::Duration)
Q_DECLARE_METATYPE(Ilwis::TimeInterval)

#endif // TIME_H
