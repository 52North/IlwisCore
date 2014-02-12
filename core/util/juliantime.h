#ifndef TIME_H
#define TIME_H

#include "Kernel_global.h"

namespace Ilwis {
/*!
 * \brief The Time class  utility class that handles all time related things within Ilwis
 *
 * The time class is build upon the notion of the Julianday (astronomical) which starts its counting at 1 januari, noon, -4713 BC. <br>
 * In Ilwis the julian day is expressed as a double so fractional days are allowed. This enables the expression of hours, minutes and seconds. <br>
 * As far as Ilwis is considered the time object is just a number so normal mathematical operations apply to it. <br>
 * To the outside world the time class is often represented as an ISO 8601 compatible string and this is also one of the notation is expects as input.
 *
 */
class KERNELSHARED_EXPORT Time {
public:
    /**
     * @brief The TimePart enum
     */
    enum TimePart{tpYEAR, tpMONTH, tpDAYOFMONTH, tpJULIANDAY, tpDAYOFTHEWEEK, tpHOUR, tpMINUTE, tpSECOND, tpDATE, tpDAYTIME};

    /**
     * @brief The Mode enum
     */
    enum Mode{mUNKNOWN, mDATE,mDATETIME,mTIME,mDURATION};

    /**
     * Creates a new time. <br>
     * If 1 of the parameters does not meet the requirements, the time object will be invalid. <br>
     * The julianday will be generated from these fields.
     *
     * @param yr the year of this time defaults to -4317
     * @param mnth the month of this time must be on [1,12], defaults to 1
     * @param dy the day of this time must fit within the month specified, defaults to 1
     * @param hr the hour of this time must be on [0,24]
     * @param min the amount of minutes of this time must be on [0,60]
     * @param sec the amount of seconds of this time must be on [0,60]
     */
    Time(int yr=-4713, int mnth=1, int dy=1, int hr=0, int min=0, double sec=0);

    /**
     * @brief Time
     * @param time
     */
    Time(const time_t time);

    /**
     * Creates a new Time from an isostring <br>
     * requires the isostring to be valid
     *
     * @param isostring the isostring with the time
     */
    //TODO link to isostring docu?
    Time(const QString& isostring);
    Time(const char * isostring);

    /**
     * Creates a new time from an amount of seconds from  0-0-0000.
     *
     * @param secondssince0 the amount of seconds after 0-0-0000
     */
    Time(double secondssince0);

    /**
     * Creates a new Time from a QDateTime object. <br>
     * This requires the relevant fields from the QDateTime to be set.
     *
     * @param time QDateTime that should be used to create this Time
     */
    Time(const QDateTime &time);

    /**
     * Deconstructor
     */
    virtual ~Time();

    /**
     * Override of the operator, returns the julianday value of this Time
     */
    operator double() const;

    /**
     * Override of the operator, generates a QDateTime object representing this Time
     */
    operator QDateTime() const;

    /**
     * Override of the = operator. sets the JulianDate of this Time to t
     *
     * @param t the new julian date of this Time
     * @return *this
     */
    Time& operator=(double t);

    /**
     * @brief toTime_t
     * @return
     */
    time_t toTime_t() const;

    /**
     * Checks whether the given year is a leapyear
     *
     * @param year the year that has to be checked, if no specified it will look at the julian time of this Time object
     * @return true if the given year is a leapyear
     */
    bool isLeapYear(int year = iUNDEF) const;

    /**
     * Query for a single field of this Time. <br>
     * You can query for:
     * - tpDATE
     * - tpDAYTIME
     * - tpYEAR
     * - tpMONTH
     * - tpDAYOFMONTH
     * - tpJULIANDAY
     * - tpDAYOFTHEWEEK
     * - tpHOUR
     * - tpMINUTE
     * - tpSECOND
     * will return rUNDEF if this Time object is invalid / the requested fields are not available
     *
     * @param part The part of this Time requested (tpX values)
     * @return The value of the requested part
     */
    virtual double get(TimePart part) const;

    /**
     * Override of the - operator substracts another time from this Time. <br>
     * The substraction is based on the Julian Time of both Times. <br>
     * Requires the Time that has to be substracted to be valid. (the time parameter)
     * if this Time is invalid it will simply return tUNDEF.
     *
     * @param time the time that should be substracted from this Time object
     * @return this after substraction
     */
    Time operator-(const Time& time) const;

    /**
     * Override of the + operator adds another time to this Time. <br>
     * The addition is based on the Julian Time of both Times. <br>
     * Requires the Time that has to be added to be valid. (the time parameter) <br>
     * if this Time is invalid it will simply return tUNDEF.
     *
     * @param time the time that should be substracted from this Time object
     * @return this after substraction
     */
    Time operator+(const Time& time) const;

    /**
     * @brief operator <
     * @param time
     * @return
     */
    bool operator <(const Time& time) const;

    /**
     * @brief operator >
     * @param time
     * @return
     */
    bool operator >(const Time& time) const;

    /**
     * @brief operator ==
     * @param time
     * @return
     */
    bool operator ==(const Time& time) const;

    /**
     * @brief operator !=
     * @param time
     * @return
     */
    bool operator !=(const Time& time) const;

    /**
     * @brief setValue
     * @param isostring
     */
    void setValue(const QString& isostring);

    /**
     * @brief setYear
     * @param year
     */
    void setYear(int year);

    /**
     * @brief setMonth
     * @param Mnt
     */
    void setMonth(int Mnt);

    /**
     * @brief setDay
     * @param dy
     */
    void setDay(int dy);

    /**
     * @brief setHour
     * @param hr
     */
    void setHour(int hr);

    /**
     * @brief setMinute
     * @param min
     */
    void setMinute(int min);

    /**
     * @brief setSecond
     * @param sec
     */
    void setSecond(double sec);

    /**
     * @brief toString
     * @param local
     * @param mode
     * @return
     */
    virtual QString toString(bool local= true, Time::Mode mode=Time::mDATETIME) const;

    /**
     * @brief isValid
     * @return
     */
    virtual bool isValid() const;

    /**
     * @brief now
     * @return
     */
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
