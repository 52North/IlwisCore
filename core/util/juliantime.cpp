#include <QDateTime>
#include <time.h>
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "numericrange.h"
#include "juliantime.h"

#define YEARDAYS 365.25
#define BIGTIME 1e150
#define NOTIME 1e15

using namespace Ilwis;

const double minDeltaTime = 1e-8;

Time::Time()
{
    _julianday = rUNDEF;
    _valid = false;
    _valuetype = itUNKNOWN;
}


void Time::checkDate(int year, int month, int day){
    if (_valid){
        if ( month >= 1 && month <= 12){ //if valid month
            if ( day >= 1 && day <= 31) { //if valid day of month
                if ( month == 2) { //if February
                    if ( isLeapYear(year) ){ // if leap year
                        if ( day > 29 ) // and dy == 30,31
                            _valid = false;
                    }else if ( day > 28 )  // if not leap year and dy == 29,30,31
                        _valid = false;
                }else if ( day == 31){
                    if (month == 4 || month == 6 || month == 9 || month == 11)// April, June, September, November
                        _valid = false;
                    // January, March, May, July, August, October, December
                }
            } else _valid = false;
        } else _valid = false;
    }
}

void Time::checkTime(int hour, int minute, double seconds){
    if(_valid){
        if ( hour < 0 || hour > 23)
            _valid = false;
        else if ( minute < 0 || minute > 59)
            _valid = false;
        else if ( seconds < 0.0 || seconds >= 60.0)
            _valid = false;
    }
}


Time::Time(int year, int month, int day, int hour, int minute, double seconds){
    _valuetype = itDATETIME;

    _valid = true;
    checkDate(year, month, day);
    checkTime(hour, minute, seconds);

    if ( _valid)
        _julianday = gregorianToJulian(year, month, day, hour, minute, seconds);
    else
        _julianday = rUNDEF;
}

Time::Time(int hour, int minute, double seconds){
    _valuetype = itTIME;

    _valid = true;
    checkTime(hour, minute, seconds);

    if ( _valid)
        _julianday = gregorianToJulian(1900, 1, 1, hour, minute, seconds);
    else
        _julianday = rUNDEF;
}

Time::Time(int year, int month, int day){
    _valuetype = itDATE;

    _valid = true;
    checkDate(year, month, day);

    if ( _valid)
        _julianday = gregorianToJulian(year, month, day, 0, 0, 0);
    else
        _julianday = rUNDEF;
}

Time::Time(const time_t tmt){
    _valid = true;
    _julianday =rUNDEF;
    _valuetype = itDATETIME;
    struct tm *time = gmtime(&tmt);
    int year = time->tm_year + 1900;
    int month = time->tm_mon;
    int day = time->tm_mday;
    int hour = time->tm_hour;
    int minutes = time->tm_min;
    int seconds = time->tm_sec;
    _julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);
}

Time::Time(const QString& isoQString) {
    _valid = true;
    _julianday = rUNDEF;
    _valuetype = itUNKNOWN;
    setValue(isoQString);
}

Time::Time(const char *isostring) : Time(QString(isostring))
{

}

Time::Time(const QDateTime& time){
    _valid = true;
    int year = time.date().year();
    int month = time.date().month();
    int day = time.date().day();
    int hour = time.time().hour();
    int minutes = time.time().minute();
    double seconds = time.time().second();
    _julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);
    _valuetype = itDATETIME;

}

Time::Time(const QDate &date) : Time(QDateTime(date))
{

}

Time::Time(const QTime &tm) : Time(QString("%1:%2:%3").arg(tm.hour()).arg(tm.minute()).arg(tm.second()))
{

}

Time::Time(const Time &time)
{
    _julianday = time._julianday;
    _valid = time._valid;
    _valuetype = time._valuetype;
}

Time::Time(const QVariant& v) : _julianday(rUNDEF),_valuetype(itUNKNOWN){
    IlwisTypes type = Domain::ilwType(v);
    _valid = true;
    if ( type == itDOUBLE){
        _valuetype = itDATETIME;
        _julianday = v.toDouble();
    }else if ( hasType(type, itINTEGER)){
        _valuetype = itDATETIME;
        _julianday = v.toDouble();
    } else if ( hasType(type, itSTRING)){
        setValue(v.toString());
    } else if ( type == itDATE){
        *this = Time(v.value<QDate>());
    } else if ( type == itTIME){
        *this = Time(v.value<QTime>());
    } else if (type == itDATETIME){
        *this = Time(v.value<QDate>());
    } else{
        QString type = v.typeName();
        if ( type == "Ilwis::Time"){
            *this = v.value<Ilwis::Time>();
        }else{
            _valid = false;
            _julianday = rUNDEF;
            _valuetype = itUNKNOWN;
        }

    }
}

Time::~Time()
{
}

Time::Time(double juliand, IlwisTypes tp) {
    _julianday = juliand;
    _valid = juliand != rUNDEF;
    _valuetype = tp;
}

Time::operator QDateTime() const{
    if ( abs(_julianday) > NOTIME)
        return QDateTime();

    int year, month, day, hour, minutes;
    double seconds;
    julianToGregorian(year,month,day,hour,minutes,seconds);
    QDateTime dt;
    dt.setDate(QDate(year,month, day));
    dt.setTime(QTime(hour,minutes,seconds));

    return dt;
}
time_t Time::toTime_t() const{
    if ( abs(_julianday) > NOTIME) {
        return (qint64) iUNDEF;
    }
    int year, month, day, hour, minutes;
    double seconds;
    julianToGregorian(year,month,day,hour,minutes,seconds);

    QDateTime t(QDate(year,month,day),QTime(hour,minutes,seconds));

    return t.toTime_t();
}

Time::operator double() const{
    return _julianday;
}

Time& Time::operator=(double t) {
    _julianday = t;
    _valid = _julianday != rUNDEF;
    return *this;
}

bool Time::isLeapYear(int year)  const{
    if ( year == iUNDEF && abs(_julianday) > NOTIME)
         return false;

     int month, day, hour, minutes;
     double seconds;

     if ( year == iUNDEF) {
         julianToGregorian(year,month,day,hour,minutes,seconds);
     }
     if((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
         return true; /* leap */
     else
         return false; /* no leap */
}

double Time::get(TimePart part) const{
    if ( _julianday == rUNDEF)
        return rUNDEF;

    if ( abs(_julianday) > NOTIME)
        return rUNDEF;

    int year, month, day, hour, minutes;
    double seconds;
    julianToGregorian(year,month,day,hour,minutes,seconds);
    if ( hasType(_valuetype, itDATE)) {
        if ( part == tpDATE) {
            QString date = QString("%1%2%3").arg(year,4,10,QLatin1Char('0')).arg(month,2,10,QLatin1Char('0')).arg(day,2,10,QLatin1Char('0'));
            return date.toDouble();
        }
        if ( part == tpDAYTIME) {
            QString date = QString("%1%2%3").arg(hour,2,10,QLatin1Char('0')).arg(minutes,2,10,QLatin1Char('0')).arg(seconds,2,'f',2,QLatin1Char('0'));
            return date.toDouble();
        }
        if ( part == tpYEAR)
            return year;
        if ( part == tpMONTH)
            return month;
        if ( part == tpDAYOFMONTH)
            return day;
        if ( part == tpJULIANDAY) {
            QDate date(year,month,day);
            return date.toJulianDay();
        }
        if ( part == tpDAYOFTHEWEEK) {
            QDate date(year,month,day);
            return date.dayOfWeek();
        }
        if ( part == tpDAYOFTHEYEAR) {
            QDate date(year,month,day);
            return date.dayOfYear();
        }
        if ( part == tpWEEKNUMBER) {
            QDate date(year,month,day);
            return date.weekNumber();
        }
    }
    if ( hasType(_valuetype, itTIME)) {
        if ( part == tpHOUR)
            return hour;
        if ( part == tpMINUTE)
            return minutes;
        if ( part == tpSECOND)
            return seconds;
    }
    return tUNDEF;
}

Duration Time::operator-(const Time& time) const {
    if ( _julianday == rUNDEF)
        return Duration();
    if ( abs(_julianday) > NOTIME)
        return Duration(_julianday, valueType());
    double t1 = time;
    double t2 = *this;
    Duration d(t2 - t1, valueType());
    return d;
}

Time Time::operator+(const Duration& time) const {
    if ( _julianday == rUNDEF)
        return tUNDEF;

    if ( abs(_julianday) > NOTIME)
        return Time(_julianday, valueType());
    double t1 = time;
    double t2 = *this;
    return Time(t2 + t1, valueType());
}

#define IGREG (14+31*(10+12*1582))
#define JGREG 2299160

int Time::tolong(double val) const{
    if ( abs(val) > NOTIME || val == rUNDEF || val == iUNDEF)
        return iUNDEF;

    if (val >= 0) {return floor(val);}
    else {return ceil(val);}
}
double Time::gregorianToJulian(int year, int month, int day, int hour, int minutes, double seconds) const {
    if ( year == iUNDEF || month == iUNDEF || day == iUNDEF || minutes == iUNDEF || seconds == rUNDEF)
        return rUNDEF;

    int orgyear = year;
    int orgmonth = month ;
    if (year<0) ++year;
    if (month > 2) ++month;
    else { --year; month+=13;}
    double jul = tolong(floor(365.25*year)+ floor(30.6001*month)+day+1720995);

    if (day+31*(orgmonth+12*orgyear) >= IGREG) {
        int ja = tolong(0.01*year);
        jul += 2-ja+tolong(0.25*ja);
    }
    double fract = (double)hour/24.0 + (double)minutes/(24.0*60.0) + seconds / (24.0*60.0*60);
    return jul + fract - 0.5;
}

void Time::julianToGregorian(int& year, int& month, int& day, int& hour, int& minutes, double& seconds) const{

    if ( _julianday == rUNDEF) {
        year = month = day = minutes = iUNDEF;
        seconds = rUNDEF;
    }

    if ( abs(_julianday) > NOTIME) {
        year = LONG_MAX; // doesnt realy matter, just an absurd large number
        month = day = 1;
        hour = minutes = seconds = 0;
        return;
    }
    year = -4713;
    month = day = 1;
    hour = minutes = seconds = 0;
    if ( _julianday == rUNDEF)
        return;

    double ja;
    double julian = tolong(_julianday + 0.5);
    double fract = _julianday - julian;
    if (julian > JGREG) {
        int jalpha = tolong(((julian-1867216)-0.25)/36524.25);
        ja=julian+1+jalpha-tolong(0.25*jalpha);
    } else { ja = julian; }
    int jb=ja+1524;
    int jc=tolong(6680+((jb-2439870)-122.1)/365.25);
    int jd=tolong(365*jc+0.25*jc);
    int je=tolong((jb-jd)/30.6001);
    day = jb-jd-tolong(30.6001*je);
    month = je-1;
    if (month >12) month -= 12;
    year = jc-4715;
    if (month > 2) --year;
    double hr = 12.0 + fract * 24.0;
    double delta =  hr - int(hr);
    hour = std::abs(delta) < 1e-6 ? int(hr + 0.5 ) : hr;
    double min = (hr - hour) * 60.0;
    delta = min - int(min);
    minutes = std::abs(delta)< 1e-6 ? int(min + 0.5) : min;
    seconds = (min - minutes) * 60.0;



}

void Time::setValue(const QString& isoQString) {
    if ( isoQString == "?") {
        _julianday = rUNDEF;
        return;
    }
    if ( isoQString == "*" || isoQString == "+*") {
        _julianday = BIGTIME;
        return;
    }
    if ( isoQString == "-*") {
        _julianday = -BIGTIME;
        return;
    }
    /*accepted iso 8601 QStrings
    YYYY[-MM[-DD[Thh[:mm[:s[Z]]]]]] ,h,m,s may be fractions. internally they are formatted to a correct int representation
    YYYY[MM[DD[Thh[:mm[:s[Z]]]]]*/
    int year=-4713, month=1, day=1, hour=0, minutes=0;
    double seconds = 0;
    parseIsoString(isoQString, year, month, day, hour, minutes,seconds);

    if ( !_valid )
        return;
    Time t(year,month,day,hour,minutes,seconds);
    _julianday = t;
}

void Time::parseOrdinalDate(int ordinal, int year, int& month, int& day) {
    int count = 0;
    int m=1;
    int prevCount = 0;
    for(; m <= 12 && count < ordinal; ++m) {
        if ( m % 2) {
            count += 31;
        } else {
            count += m==2 && !isLeapYear(year) ? 28 : 29;
        }
        prevCount = count;
    }
    month = --m;
    day = ordinal - prevCount;
}

void Time::parseYearPart(const QString& yearpart, int& year, int& month, int& day) {
    QStringList parts= yearpart.split("-");
    if ( parts.size() < 2) {
        year = yearpart.mid(0,4).toLong();
        if ( yearpart.size() >4)
            month = yearpart.mid(4,2).toLong();
        if ( yearpart.size() > 6)
            day = yearpart.mid(6,2).toLong();
    } else {
        year = parts[0].toLong();
        if ( parts.size() > 1) {
            if (parts[1].size() == 3)
                parseOrdinalDate(parts[1].toLong(), year, month, day);
            month = parts[1].toLong();
        }
        if ( parts.size() > 2)
            day = parts[2].toLong();
    }
    if ( month > 12 || day >31){
        _valid = false;
    }
}

void Time::parseDayPart(const QString& daypart, int& hours, int& minutes, double& seconds) {
    QStringList parts = daypart.split(":");
    double hr=0, min=0, sec=0;
    if ( parts.size() < 2) {
        if ( daypart[2] == '.')
            hr = daypart.toDouble();
        else{
            hr = daypart.mid(0,2).toDouble();
            if ( daypart.size() >= 3) {
                if ( daypart[2] == '.') {
                    min = daypart.mid(2, daypart.size() - 2).toDouble();
                } else {
                    min = daypart.mid(2,2).toDouble();
                    if ( daypart.size() >= 5) {
                        sec = daypart.mid(4, daypart.size() - 3).toDouble();
                    }
                }
            }
        }

    } else {
        hr = parts[0].toDouble();
        if ( parts.size() > 1)
            min = parts[1].toDouble();
        if ( parts.size() > 2)
            sec = parts[2].toDouble();
    }
    if ( hr == rUNDEF || min == rUNDEF || sec == rUNDEF) {
        _valid = false;
        return;
    }
    if (( hr - (int)hr) > 0)
        min += (hr - (int)hr) * 60;
    else if (( min - (int)min) > 0) {
        sec += (min - (int)min) * 60;
    }
    hours = hr;
    minutes = min;
    seconds = sec;
}

void Time::parseIsoString(const QString& isoQString, int& year, int& month, int& day, int& hours, int& minutes, double& seconds) {
    year=-4713, month=1, day=1, hours=0, minutes=0;
    seconds = 0;
    if ( isoQString == "?") {
        _julianday = rUNDEF;
        return;
    }
    if ( isoQString == "*" || isoQString == "+*")
        _julianday = BIGTIME;
    if ( isoQString == "-*")
        _julianday = -BIGTIME;
    // special form YYYYMMDDHHMM without the T marker for time, often used in sattelite orbit times
    _valuetype = itDATETIME;
    if ( isoQString.size() == 12 && isoQString.indexOf("T") == -1 &&
        isoQString.indexOf("-") == -1 &&
        isoQString.indexOf(":") == -1 &&
        isoQString.indexOf(".") == -1)
    {
        parseYearPart(isoQString.mid(0,8), year, month, day);
        parseDayPart(isoQString.mid(8,6),hours, minutes, seconds);
    } else{
        QString yearpart = isoQString.split("T").front();

        QString daypart = isoQString.split("T").back().split("Z").front();
        bool hasDate = yearpart != "" && yearpart.indexOf(":") == -1;
        if ( hasDate ){
            parseYearPart(yearpart,year, month,day);
        } else
            _valuetype = itTIME;

        bool hasTime = daypart != "" && (( isoQString.indexOf("T") >= 0) || (daypart.indexOf(":")>0 ));
        if ( hasTime) {
            parseDayPart(daypart, hours, minutes, seconds);
        }else
            _valuetype = _valuetype == itDATETIME ? itDATE : itUNKNOWN;
    }
}

bool Time::operator <(const Time& time) const{
    if ( _julianday == rUNDEF)
        return false;

    return _julianday < (double)time;
}
bool Time::operator >(const Time& time) const{
    if ( _julianday == rUNDEF)
        return false;

    return _julianday > (double)time;
}
bool Time::operator ==(const Time& time) const{
    if (!_valid && !time._valid)
        return true;
    if ( _julianday == rUNDEF && time._julianday == rUNDEF)
        return true;

    if ( _julianday == rUNDEF)
        return false;

    return std::abs(_julianday - (double)time) <= minDeltaTime;
}

bool Time::operator !=(const Time& time) const{
    if ( _julianday == rUNDEF)
        return false;

    return _julianday != (double)time;
}

bool Time::isValid() const {
    if ( _julianday == rUNDEF)
        return false;

    return _valid && _valuetype != itUNKNOWN;
}

IlwisTypes Time::valueType() const
{
    return _valuetype;
}

void Time::valueType(IlwisTypes tp)
{
    _valuetype = tp;
}

void Time::setYear(int yr) {
    if ( yr == iUNDEF) {
        _julianday = rUNDEF;
        return;
    }
    int year, month, day, hour, minutes;
    double seconds;
    julianToGregorian(year,month,day,hour,minutes,seconds);
    year = yr;
    _julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);
    if ( _valuetype == itTIME)
        _valuetype = itDATETIME;
}
void Time::setMonth(int mnth){
    if ( mnth == iUNDEF) {
        _julianday = rUNDEF;
        return;
    }
    int year, month, day, hour, minutes;
    double seconds;
    julianToGregorian(year,month,day,hour,minutes,seconds);
    if ( mnth >= 1 && mnth <= 12)
        month = mnth;
    else
        _valid = false;
    if ( _valid){
        _julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);
        if ( _valuetype == itTIME)
            _valuetype = itDATETIME;
    }
}

void Time::setDay(int dy) {
    if ( dy == iUNDEF) {
        _julianday = rUNDEF;
        return;
    }
    int year, month, day, hour, minutes;
    double seconds;
    julianToGregorian(year,month,day,hour,minutes,seconds);
    if ( dy >= 1 && dy <= 31) {
        if ( month == 2) {
            if ( dy <= 28 && isLeapYear()) {
                day = dy;
            } else if( dy <= 27 && !isLeapYear())
                day = dy;
            else
                _valid = false;
        }else {
            if ( (month - 1) % 2 == 0 && dy <= 30)
                day = dy;
            else if ( dy <= 29)
                day = dy;
            else
                _valid = false;
        }
    }
    if ( _valid){
        _julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);
        if ( _valuetype == itTIME)
            _valuetype = itDATETIME;
    }
}

void Time::setHour(int hr) {
    if ( hr == iUNDEF) {
        _julianday = rUNDEF;
        return;
    }
    int year, month, day, hour, minutes;
    double seconds;
    julianToGregorian(year,month,day,hour,minutes,seconds);
    if ( hr >= 0 && hr <= 23)
        hour = hr;
    else
        _valid = false;
    if ( _valid){
        _julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);
        if ( _valuetype == itDATE)
            _valuetype = itDATETIME;
    }

}

void Time::setMinute(int min) {
    if ( min == iUNDEF) {
        _julianday = rUNDEF;
        return;
    }
    int year, month, day, hour, minutes;
    double seconds;
    julianToGregorian(year,month,day,hour,minutes,seconds);
    if ( min >= 0 && min <= 59)
        minutes = min;
    else
        _valid = false;
    if ( _valid){
        _julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);
        if ( _valuetype == itDATE)
            _valuetype = itDATETIME;
    }

}

void Time::setSecond(double sec) {
    if ( sec == rUNDEF || sec == iUNDEF) {
        _julianday = rUNDEF;
        return;
    }
    int year, month, day, hour, minutes;
    double seconds;
    julianToGregorian(year,month,day,hour,minutes,seconds);
    if ( sec >= 0.0 && sec < 60.0 ) {
        seconds = sec;
    }
    else {
        _valid = false;
    }
    if ( _valid){
        _julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);
        if ( _valuetype == itDATE)
            _valuetype = itDATETIME;
    }
}



QString Time::toString(bool local, IlwisTypes tp) const{
    if ( tp == itUNKNOWN)
        tp = _valuetype;
    if ( *this == tUNDEF || _julianday == rUNDEF)
        return "?";
    if ( abs(_julianday) > NOTIME && _julianday > 0)
        return "+*";
    if ( abs(_julianday) > NOTIME && _julianday < 0)
        return "-*";

    int year, month, day, hour, minutes;
    double seconds;
    julianToGregorian(year,month,day,hour,minutes,seconds);
    if ( year == -4172 && tp != itTIME )
        return "?";
    struct tm time;
    time.tm_year = isLeapYear() ? 96 : 97;
    time.tm_mon  = month - 1 ; // 0 based
    time.tm_mday = day;
    time.tm_hour = hour;
    time.tm_min = minutes;
    time.tm_sec = round(seconds);
    time_t t = mktime(&time);
    if ( local) {
        struct tm *timeinfo = localtime(&t);
        if ( tp == itDATE)
            return QString("%1-%2-%3").arg(year,4, 10, QLatin1Char('0')).arg(timeinfo->tm_mon + 1,2,10,QLatin1Char('0')).arg(timeinfo->tm_mday,2,10,QLatin1Char('0'));
        else if ( tp == itDATETIME)
            return QString("%1-%2-%3T%4:%5:%6").arg(year,4, 10, QLatin1Char('0')).arg(timeinfo->tm_mon + 1,2,10,QLatin1Char('0')).arg(timeinfo->tm_mday,2,10,QLatin1Char('0')).arg(timeinfo->tm_hour,2,10,QLatin1Char('0')).arg(timeinfo->tm_min,2,10,QLatin1Char('0')).arg(timeinfo->tm_sec,2,10,QLatin1Char('0'));
        else if ( tp == itTIME)
            return QString("%1:%2:%3").arg(timeinfo->tm_hour,2,10,QLatin1Char('0')).arg(timeinfo->tm_min,2,10,QLatin1Char('0')).arg(seconds,2 ,'f',2 ,QLatin1Char('0'));
    } else {
        //struct tm *timeinfo = gmtime(& t);
        if ( tp == itDATE)
            return QString("%1-%2-%3").arg(year,4, 10, QLatin1Char('0')).arg(month,2,10,QLatin1Char('0')).arg(day,2,10,QLatin1Char('0'));
        else if ( tp == itDATETIME)
            return QString("%1-%2-%3T%4:%5:%6").arg(year,4,10,QLatin1Char('0')).arg(month,2,10,QLatin1Char('0')).arg(day,2,10,QLatin1Char('0')).arg(hour,2,10,QLatin1Char('0')).arg(minutes,2,10,QLatin1Char('0')).arg(seconds,2, 'f',2 ,QLatin1Char('0'));
        else if ( tp == itTIME)
            return QString("%1:%2:%3").arg(hour,2, 10, QLatin1Char('0')).arg(minutes,2,10,QLatin1Char('0')).arg(seconds,2,'f',2 ,QLatin1Char('0'));
    }
    return "?";
}

Time Time::now() {
    return Time(QDateTime::currentDateTime());
}
//-------------------------------------------
Duration::Duration(const QString& duration, IlwisTypes tp) {
    QString definition = duration;
    QHash<QString, double> values;
    _valuetype = tp;
    values["Y"] = -4713;
    values["M"] = 1.0;
    values["D"] = 1.0;
    values["h"] = 0.0;
    values["m"] = 0.0;
    values["s"] = 0.0;

    QString v;
    double rest = 0;
    for(int i=0; i < definition.size(); ++i) {
        char c = definition[i].toLatin1();
        if ( (c >= QLatin1Char('0') && c <= '9') || c == '.')
            v+=c;
        else if ( c == 'Y' || c == 'M'|| c == 'D' || c == 'h' || c == 'm' ||c == 's') {
            double value = v.toDouble();
            values[QString(c)] += value;
            v = "";
        }
        else if ( c == 'T') {
            v = "";
        }
    }
    int year, month, day, hour, minute;
    double seconds;
    year = values["Y"];
    seconds = values["s"];
    minute = values["m"];
    hour = values["h"] + 12;
    day = values["D"];
    month = values["M"];

    if ( seconds >= 60) {
        minute += ((int)seconds) % 60;
        seconds = seconds - minute * 60.0;
    }
    if ( minute >= 60) {
        hour += minute % 60;
        minute = minute - hour * 60;
    }
    if ( hour > 24) {
        day += hour % 24;
        hour = hour - day * 24;
    }
    if (  day > 365) {
        year += day / 365;
        day = day % 365;
    }
    if ( month > 12) {
        year += month % 12;
        month = month - year * 12;
    }
    _julianday = gregorianToJulian(year, month, day, hour, minute, seconds);
    _julianday += rest;
}

Duration::Duration(double r, IlwisTypes tp) : Time(r) {
    if ( r == rUNDEF || r == tUNDEF || r == 0)
        _valid = false;
    _valid = true;
    _valuetype = tp;
}

QString Duration::toString(bool local, IlwisTypes tp ) const{
    if ( *this == tUNDEF || _julianday == rUNDEF)
        return "";
    int year, month, day, hour, minutes;
    double seconds;
    julianToGregorian(year,month,day,hour,minutes,seconds);
    // time says year, month, day have a minimum of 1 and go up to max but ofc for duration this is not true
    // duration goes from 0-max-1;
    year += 4712;
    month -= 1;
    day -= 1;

    QString dur = "P";
    if ( year != 0)
        dur += QString("%1Y").arg(year);
    if ( month != 0)
        dur += QString("%1M").arg(month);
    if ( day != 0)
        dur+= QString("%1D").arg(day);

    QString dur2 = "T";
    if ( hour != 0)
        dur2 += QString("%1H").arg(hour);
    if ( minutes != 0)
        dur2 += QString("%1M").arg(minutes);
    if ( seconds !=0)
        dur2 += QString("%1S").arg(seconds);
    if ( dur2 != "T")
        dur += "T" + dur2;


    return dur == "P" ? "" : dur;

}

double Duration::get(TimePart part) const{
    if ( _julianday == rUNDEF)
        return rUNDEF;

    if ( abs(_julianday) > NOTIME)
        return rUNDEF;

    int year, month, day, hour, minutes;
    double seconds;
    julianToGregorian(year,month,day,hour,minutes,seconds);
    if ( month == 12) { // month 12 is a duration of 1 year, month 12 makes sense in Time's context, but not in duration;
        ++year;
        month = 0;
    }
    if ( part == tpYEAR)
        return year + 4712;
    else if (part == tpMONTH)
        return month - 1; // in time months are 1 based, but for duration this doesnt make sense
    else if (part == tpDAYOFMONTH )
        return day - 1; // in time days are 1 based, but for duration this doesnt make sense
    else{
        return Time::get(part);
    }
    return tUNDEF;
}


bool Duration::isValid() const{
    return Time::isValid() && _julianday != 0;
}


//-------------------------------------------
TimeInterval::TimeInterval(IlwisTypes tp) : NumericRange(-BIGTIME, BIGTIME,0){
    _step = Duration(tUNDEF);
    _vt = tp;
}

TimeInterval::TimeInterval(const Time& beg, const Time& en, const Duration& stp, IlwisTypes tp) :
NumericRange((double)beg, (double)en),
_step(stp)
{
    if ( (double)beg == BIGTIME) {
        min(-BIGTIME);
    }
    if ( (double)_step != 0)
        resolution(_step);
    _vt = tp;
    if ( tp == itUNKNOWN){
        _vt = itDATETIME;
        if ( beg.valueType() == itDATE && en.valueType() == itDATE)
            _vt = itDATE;
        if ( beg.valueType() == itTIME && en.valueType() == itTIME)
            _vt = itTIME;
        if ( (beg.valueType() == itTIME && en.valueType() == itDATE) ||
             (beg.valueType() == itDATE && en.valueType() == itTIME))
            _vt = itUNKNOWN;
    }

}

QString TimeInterval::toString(bool local, IlwisTypes tp) const{
    if (min() == rUNDEF || max() == rUNDEF)
        return sUNDEF;

    Time begin(min(), tp);
    Time end(max(), tp);
    QString sb = begin.toString(local,tp);
    QString se = end.toString(local,tp);
    return QString("timeinterval:%1|%2").arg(sb ,se);
}

bool TimeInterval::contains(const QString &value, bool inclusive) const
{
    if (min() == rUNDEF || max() == rUNDEF)
        return false;

    Time t(value);
    if ( !t.isValid())
        return false;
    return NumericRange::contains(t, inclusive);

}

bool TimeInterval::contains(const QVariant& value, bool inclusive) const{
    if (min() == rUNDEF || max() == rUNDEF)
        return false;
    if (!value.isValid() || value.isNull())
        return false;

    QString type = value.typeName();
    if ( type!= "QString" && type != "Ilwis::Time" && type != "double" && type != "QDateTime" && type != "QDate" && type != "QTime"){
        ERROR2(ERR_COULD_NOT_CONVERT_2,value.toString(), "time");
        return false;
    }
    Time t;
    if ( type == "Ilwis::Time")
        t = value.value<Ilwis::Time>();
    else if ( type == "double")
        t = Time(value.toDouble());
    else if ( type == "QDateTime")
        t = Time(value.toDateTime());
    else if ( type == "QTime")
        t = Time(value.toTime());
    else if ( type == "QDate")
        t = Time(value.toDate());
    else if (type == "QString")
        t = Time(value.toString());


    return contains(t, inclusive);

}

bool TimeInterval::contains(const Time &value, bool inclusive) const
{
    return NumericRange::contains(value, inclusive);
}

QVariant TimeInterval::impliedValue(const QVariant &v) const
{

    Time t(v);
    QVariant tvalue;
    tvalue.setValue<Ilwis::Time>(t);
    return ensure(tvalue);
}

QVariant TimeInterval::ensure(const QVariant &v, bool inc) const
{
    Time temp = v.value<Ilwis::Time>();
    Time t(NumericRange::ensure((double)temp,inc));
    if (!t.isValid())
        return QVariant();
    t.valueType(temp.valueType());
    QVariant timev;
    timev.setValue<Ilwis::Time>(t);
    return timev;
}

IlwisTypes TimeInterval::valueType() const
{
    return _vt;
}

void TimeInterval::add(const QVariant &time)
{
    if ( contains(time))
        return;
    Time t = impliedValue(time).value<Ilwis::Time>();
    NumericRange::add((double)t);
}

void TimeInterval::store(QDataStream &stream)
{
    NumericRange::store(stream);

    stream << (double)_step << _vt;
}

void TimeInterval::load(QDataStream &stream)
{
    NumericRange::load(stream);
    double step;
    stream >> step >> _vt;
    _step = step;
}

Range *TimeInterval::clone() const
{
    return new TimeInterval(min(), max(), _step, _vt);
}

bool TimeInterval::isValid() const
{
    bool ok = NumericRange::isValid();
    return ok &&  valueType() != itUNKNOWN;
}


TimeInterval& TimeInterval::operator=(const TimeInterval& tiv){
    min(tiv.min());
    max(tiv.max());
    _step = tiv.getStep();
    _vt = tiv._vt;

    return *this;
}

void TimeInterval::begin(const Time &t)
{
    NumericRange::min(t);
}

void TimeInterval::end(const Time &t)
{
    NumericRange::max(t);
}

double TimeInterval::valueAt(quint32& index, const Range *rng)
{
    return NumericRange::valueAt(index, rng) ;
}

QString TimeInterval::valueAsString(quint32 &index, const Range *rng)
{
    double value = TimeInterval::valueAt(index, rng);
    return Time(value).toString();
}









