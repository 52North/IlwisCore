#ifndef TIME_H
#define TIME_H

#include "kernel_global.h"

namespace Ilwis {

class Duration;
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
    enum TimePart{tpYEAR, tpMONTH, tpDAYOFMONTH, tpJULIANDAY, tpDAYOFTHEWEEK, tpDAYOFTHEYEAR, tpWEEKNUMBER, tpHOUR, tpMINUTE, tpSECOND, tpDATE, tpDAYTIME};

    Time();
    /**
     * Creates a new time of type itDATETIME <br>
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
    Time(int year, int month, int day, int hour, int minute, double seconds=0);
    /**
     * Creates a new time of type itTIME <br>
     * If 1 of the parameters does not meet the requirements, the time object will be invalid. <br>
     * The julianday will be generated from these fields.
     *
     * @param hr the hour of this time must be on [0,24]
     * @param min the amount of minutes of this time must be on [0,60]
     * @param sec the amount of seconds of this time must be on [0,60]
     */
    Time(int hour, int minute, double seconds=0);
    /**
     * Creates a new time of type itDATE <br>
     * If 1 of the parameters does not meet the requirements, the time object will be invalid. <br>
     * The julianday will be generated from these fields.
     *
     * @param yr the year of this time defaults to -4317
     * @param mnth the month of this time must be on [1,12], defaults to 1
     * @param dy the day of this time must fit within the month specified, defaults to 1
     */
    Time(int year, int month=1, int day=1);

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
    Time(const QString& isostring);
    Time(const char * isostring);

    /**
     * Creates a new time from an amount of seconds from  0/0/-4712
     *
     * @param days the amount of days after 0/0/-4712
     */
    Time(double jd, IlwisTypes m =itDATETIME);

    /**
     * Creates a new Time from a QDateTime object. <br>
     * This requires the relevant fields from the QDateTime to be set.
     *
     * @param time QDateTime that should be used to create this Time
     */
    Time(const QDateTime &time);
    Time(const QDate& date);
    Time(const QTime& tm);
    Time(const Time& time);
    Time(const QVariant &v);

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
    Duration operator-(const Time& time) const;

    /**
     * Override of the + operator adds another time to this Time. <br>
     * The addition is based on the Julian Time of both Times. <br>
     * Requires the Time that has to be added to be valid. (the time parameter) <br>
     * if this Time is invalid it will simply return tUNDEF.
     *
     * @param time the time that should be added from this Time object
     * @return this after substraction
     */
    Time operator+(const Duration& duration) const;

    /**
     * Override of the < operator compares this time with another. <br>
     * The comparison is based on the Julian Time of both Times. <br>
     * Requires the Time that has to be added to be valid. (the time parameter) <br>
     * if this Time is invalid it will simply return false.
     *
     * @param time the time that should be compared with this Time object
     * @return true when the juliandate of this time is smaller than the other Time 's
     */
    bool operator <(const Time& time) const;

    /**
     * Override of the > operator compares this time with another. <br>
     * The comparison is based on the Julian Time of both Times. <br>
     * Requires the Time that has to be added to be valid. (the time parameter) <br>
     * if this Time is invalid it will simply return false.
     *
     * @param time the time that should be compared with this Time object
     * @return true when the juliandate of this time is bigger than the other Time 's
     */
    bool operator >(const Time& time) const;

    /**
     * Override of the == operator compares this time with another. <br>
     * The comparison is based on the Julian Time of both Times. <br>
     * Requires the Time that has to be added to be valid. (the time parameter) <br>
     * if this Time is invalid it will simply return false.
     *
     * @param time the time that should be compared with this Time object
     * @return true when the juliandate of this time is equal to0 the other Time 's
     */
    bool operator ==(const Time& time) const;

    /**
     * Override of the != operator compares this time with another. <br>
     * The comparison is based on the Julian Time of both Times. <br>
     * Requires the Time that has to be added to be valid. (the time parameter) <br>
     * if this Time is invalid it will simply return false.
     *
     * @param time the time that should be compared with this Time object
     * @return true when the juliandate of this time is not equal to the other Time 's
     */
    bool operator !=(const Time& time) const;

    /**
     * Generates the fields of this time from an iso string. <br>
     * accepted iso 8601 QStrings:
     *
     * - YYYY[-MM[-DD[Thh[:mm[:s[Z]]]]]]
     * - YYYY[MM[DD[Thh[:mm[:s[Z]]]]]
     *
     * h,m,s may be fractions. internally they are formatted to a correct int representation
     * @param isostring
     */
    void setValue(const QString& isostring);

    /**
     * Sets the year field of this time. <br>
     * if year is iUNDEF the julianday will be set to rUNDEF. <br>
     * If you want to set the fields 1 by 1 you have to start at the biggest field or else they will override each other.
     *
     * @param year the new year of this Time
     */
    void setYear(int year);

    /**
     * Sets the month field of this time. <br>
     * if month is iUNDEF the julianday will be set to rUNDEF. <br>
     * If you want to set the fields 1 by 1 you have to start at the biggest field or else they will override each other.
     *
     * @param year the new year of this Time
     */
    void setMonth(int Mnt);

    /**
     * Sets the day field of this time. <br>
     * if day is iUNDEF the julianday will be set to rUNDEF. <br>
     * If you want to set the fields 1 by 1 you have to start at the biggest field or else they will override each other.
     *
     * @param year the new year of this Time
     */
    void setDay(int dy);

    /**
     * Sets the hour field of this time. <br>
     * if hour is iUNDEF the julianday will be set to rUNDEF. <br>
     * If you want to set the fields 1 by 1 you have to start at the biggest field or else they will override each other.
     *
     * @param year the new year of this Time
     */
    void setHour(int hr);

    /**
     * Sets the minute field of this time. <br>
     * if minute is iUNDEF the julianday will be set to rUNDEF. <br>
     * If you want to set the fields 1 by 1 you have to start at the biggest field or else they will override each other.
     *
     * @param year the new year of this Time
     */
    void setMinute(int min);

    /**
     * Sets the second field of this time. <br>
     * if second is iUNDEF the julianday will be set to rUNDEF. <br>
     * If you want to set the fields 1 by 1 you have to start at the biggest field or else they will override each other.
     *
     * @param year the new year of this Time
     */
    void setSecond(double sec);

    /**
     * Translates this type to a string format of one of the following types:
     * - itDATE
     * - itDATETIME
     * - itTIME
     *
     * The typ defaults to the value type of this time
     *
     * @param local
     * @param mode
     * @return
     */
    virtual QString toString(bool local= false, IlwisTypes tp=itUNKNOWN) const;

    /**
     * Checks the validity of this time. <br>
     * A time is invalid when there is no time set, or 1 or more of the inner members have an invalid value (negative value).
     *
     * @return true if this time is valid (the value within this time)
     */
    virtual bool isValid() const;

    /**
     * Calculates the current time using the time of the system.
     *
     * @return the time at the moment of the call
     */
    static Time now();

    //@override
    IlwisTypes valueType() const;

    //@override
    void valueType(IlwisTypes tp);

    double gregorianToJulian(int year, int month, int day, int hour, int minutes, double seconds) const;

protected:
    void checkDate(int year, int month, int day); // alters _valid
    void checkTime(int hour, int minute, double seconds); //alters _valid

    void julianToGregorian(int& year, int& month, int& day, int& hour, int& minutes, double& seconds) const;
    int julianyearday(int year, int month, int day) const;
    int tolong(double val) const;
    void parseOrdinalDate(int ordinal, int year, int& month, int& day);
    void parseYearPart(const QString& yearpart, int& year, int& month, int& day) ;
    void parseDayPart(const QString& dayPart, int& hours, int& minutes, double& seconds);
    void parseIsoString(const QString& isostring, int& year, int& month, int& day, int& hours, int& minutes, double& seconds);

    double _julianday;
    bool _valid;
    IlwisTypes _valuetype;
};

class KERNELSHARED_EXPORT Duration : public Time {
public:
    Duration(const QString& step="",IlwisTypes tp=itTIME);
    Duration(double r,IlwisTypes tp=itTIME);
    QString toString(bool local, IlwisTypes tp=itTIME) const;
    bool isValid() const;
    double get(TimePart part) const;
};

class KERNELSHARED_EXPORT TimeInterval : public NumericRange {
public:
    TimeInterval(IlwisTypes tp=itUNKNOWN);
    TimeInterval(const Time& begin, const Time& end, const Duration& stp=Duration(""), IlwisTypes tp=itUNKNOWN);
    TimeInterval& operator=(const TimeInterval& tiv);
    Time begin() const { return Time(min());}
    Time end() const { return Time(max());}

    void begin(const Time& t) ;
    void end(const Time& t);
    Duration getStep() const { return _step;}
    QString toString(bool local, IlwisTypes) const;
    bool contains(const QString& value, bool inclusive = true) const;
    bool contains(const Time& value, bool inclusive = true) const;
    bool contains(const QVariant& value, bool inclusive = true) const;
    QVariant impliedValue(const QVariant& v) const;
    QVariant ensure(const QVariant& v, bool inclusive=true) const;
    IlwisTypes valueType() const;
    static double valueAt(quint32& index, const Range *rng);
    static QString valueAsString(quint32 &index, const Range *rng);
    void add(const QVariant &time);
    void store(QDataStream& stream);
    void load(QDataStream& stream);

    Range *clone() const ;
    bool isValid() const;

private:
    Duration _step;
    IlwisTypes _vt;
};

#define tUNDEF Time()

}

Q_DECLARE_METATYPE(Ilwis::Time)
Q_DECLARE_METATYPE(Ilwis::Duration)
Q_DECLARE_METATYPE(Ilwis::TimeInterval)

#endif // TIME_H
