#include <QDateTime>
#include <time.h>
#include "kernel.h"
#include "range.h"
#include "numericrange.h"
#include "juliantime.h"

#define YEARDAYS 365.25
#define BIGTIME 1e150
#define NOTIME 1e15

using namespace Ilwis;

Time::Time(int yr, int mnth, int dy, int hr, int min, double sec)
{
    _valid = true;
    _julianday = rUNDEF;

    int year = yr;
    int month;
    if ( mnth >= 1 && mnth <= 12)
        month = mnth;
    else
        _valid = false;

    int day;
    if ( dy >= 1 && dy <= 31 && _valid) {
        if ( month == 2) {
            if ( dy <= 29 && isLeapYear(yr)) {
                day = dy;
            } else if( dy <= 28 && !isLeapYear(yr))
                day = dy;
            else
                _valid = false;
        }else {
            if ( month < 8 && (month + 1) % 2 == 0 && dy <= 31)
                day = dy;
            else if ( month >= 8 && month % 2 == 0 && dy <= 31)
                day = dy;
            else if ( dy <= 30)
                day = dy;
            else
                _valid = false;
        }
    } else
        _valid = false;

    int hour;
    if ( hr >= 0 && hr <= 23 && _valid)
        hour = hr;
    else
        _valid = false;

    int minutes;
    if ( min >= 0 && min <= 59 && _valid)
        minutes = min;
    else
        _valid = false;

    double seconds;
    if ( sec >= 0.0 && sec < 60.0  && _valid) {
        seconds = sec;
    }
    else {
        _valid = false;
    }
    if ( _valid)
        _julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);
    else
        _julianday = rUNDEF;
}
Time::Time(const time_t tmt){
    _valid = true;
    _julianday =rUNDEF;
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
    setValue(isoQString);
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

}

Time::~Time()
{
}

Time::Time(double juliand) {
    _julianday = juliand;
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
    struct tm time;
    if ( abs(_julianday) > NOTIME) {
        return (__int64) iUNDEF;
    }
    int year, month, day, hour, minutes;
    double seconds;
    julianToGregorian(year,month,day,hour,minutes,seconds);

    time.tm_year = year - 1900;
    time.tm_mon  = month ;
    time.tm_mday = day;
    time.tm_hour = hour;
    time.tm_min = minutes;
    time.tm_sec = round(seconds);
    return mktime(&time);
}

Time::operator double() const{
    return _julianday;
}

Time& Time::operator=(double t) {
    _julianday = t;
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
    if ( part == tpDATE) {
        QString date = QString("%1%2%3").arg(year,4,10,QLatin1Char('0')).arg(month,2,10,QLatin1Char('0')).arg(day,2,10,QLatin1Char('0'));
        return date.toDouble();
    }
    if ( part == tpDAYTIME) {
        QString date = QString("%1%2%3").arg(hour,2,10,QLatin1Char('0')).arg(minutes,2,10,QLatin1Char('0')).arg(seconds,2,'g',2,QLatin1Char('0'));
        return date.toDouble();
    }
    if ( part == tpYEAR)
        return year;
    if ( part == tpMONTH)
        return month;
    if ( part == tpDAYOFMONTH)
        return day;
    if ( part == tpJULIANDAY) {
        int tempYear = year;
        year = isLeapYear() ? 1980 : 1981;
        time_t p = toTime_t();
        struct tm *time = gmtime(&p);
        year = tempYear;
        return time->tm_wday;
    }
    if ( part == tpDAYOFTHEWEEK) {
        int tempYear = year;
        year = isLeapYear() ? 1980 : 1981;
        time_t p = toTime_t();
        struct tm *time = gmtime(&p);
        year = tempYear;
        return time->tm_yday;
    }
    if ( part == tpHOUR)
        return hour;
    if ( part == tpMINUTE)
        return minutes;
    if ( part == tpSECOND)
        return seconds;
    return tUNDEF;
}

Time Time::operator-(const Time& time) const {
    if ( _julianday == rUNDEF)
        return tUNDEF;
    if ( abs(_julianday) > NOTIME)
        return _julianday;
    double t1 = time;
    double t2 = *this;
    return Time(t2 - t1);
}

Time Time::operator+(const Time& time) const {
    if ( _julianday == rUNDEF)
        return tUNDEF;

    if ( abs(_julianday) > NOTIME)
        return _julianday;
    double t1 = time;
    double t2 = *this;
    return Time(t2 + t1);
}
//TODO defines up on top?
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
    return jul + fract;
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
    double julian = tolong(_julianday);
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
    double hr = fract * 24.0;
    double delta =  hr - int(hr + 0.5);
    hour = abs(delta) < 1e-6 ? int(hr + 0.5) : hr;
    double min = (hr - hour) * 60.0;
    delta = min - int(min + 0.5);
    minutes = abs(delta)< 1e-6 ? int(min + 0.5) : min;
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
    int prevCount;
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
    if ( isoQString.size() == 12 && isoQString.indexOf("T") == -1 &&
        isoQString.indexOf("-") == -1 &&
        isoQString.indexOf(":") == -1 &&
        isoQString.indexOf(".") == -1)
    {
        parseYearPart(isoQString.mid(0,8), year, month, day);
        parseDayPart(isoQString.mid(8,6),hours, minutes, seconds);
    } else{
        QString yearpart = isoQString.split("T").front();
        QString daypart = isoQString.split("T").front().split("Z").front();
        if ( yearpart != "")
            parseYearPart(yearpart,year, month,day);
        if ( daypart != "")
            parseDayPart(daypart, hours, minutes, seconds);
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
    if ( _julianday == rUNDEF)
        return false;

    return _julianday == (double)time;
}

bool Time::operator !=(const Time& time) const{
    if ( _julianday == rUNDEF)
        return false;

    return _julianday != (double)time;
}

bool Time::isValid() const {
    if ( _julianday == rUNDEF)
        return false;

    return _valid;
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
    if ( _valid)
        _julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);
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
    if ( _valid)
        _julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);
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
    if ( _valid)
        _julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);

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
    if ( _valid)
        _julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);

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
    if ( _valid)
        _julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);
}



QString Time::toString(bool local, Time::Mode mode) const{
    if ( *this == tUNDEF || _julianday == rUNDEF)
        return "?";
    if ( abs(_julianday) > NOTIME && _julianday > 0)
        return "+*";
    if ( abs(_julianday) > NOTIME && _julianday < 0)
        return "-*";

    int year, month, day, hour, minutes;
    double seconds;
    julianToGregorian(year,month,day,hour,minutes,seconds);
    if ( year == -4172 && mode != mTIME )
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
        if ( mode == mDATE)
            return QString("%1-%2-%3").arg(year,4, 10, QLatin1Char('0')).arg(timeinfo->tm_mon + 1,2,10,QLatin1Char('0')).arg(timeinfo->tm_mday,2,10,QLatin1Char('0'));
        else if ( mode == mDATETIME)
            return QString("%1-%2-%3T%4:%5:%6").arg(year,4, 10, QLatin1Char('0')).arg(timeinfo->tm_mon + 1,2,10,QLatin1Char('0')).arg(timeinfo->tm_mday,2,10,QLatin1Char('0')).arg(timeinfo->tm_hour,2,10,QLatin1Char('0')).arg(timeinfo->tm_min,2,10,QLatin1Char('0')).arg(timeinfo->tm_sec,2,10,QLatin1Char('0'));
        else if ( mode == mTIME)
            return QString("T%1:%2:%3").arg(timeinfo->tm_hour,2,10,QLatin1Char('0')).arg(timeinfo->tm_min,2,10,QLatin1Char('0')).arg(seconds,'g',2 , 2,QLatin1Char('0'));
    } else {
        //struct tm *timeinfo = gmtime(& t);
        if ( mode == mDATE)
            return QString("%1-%2-%3").arg(year,4, 10, QLatin1Char('0')).arg(month,2,10,QLatin1Char('0')).arg(day,2,10,QLatin1Char('0'));
        else if ( mode == mDATETIME)
            return QString("%1-%2-%3T%4:%5:%6").arg(year,4,10,QLatin1Char('0')).arg(month,2,10,QLatin1Char('0')).arg(day,2,10,QLatin1Char('0')).arg(hour,2,10,QLatin1Char('0')).arg(minutes,2,10,QLatin1Char('0')).arg(seconds,'g',2 , 2,QLatin1Char('0'));
        else if ( mode == mTIME)
            return QString("T%1:%2:%3").arg(hour,2, 10, QLatin1Char('0')).arg(minutes,2,10,QLatin1Char('0')).arg(seconds,'g',2 , 2,QLatin1Char('0'));
    }
    return "?";
}

Time Time::now() {
    return Time(QDateTime::currentDateTime());
}
//-------------------------------------------
Duration::Duration(const QString& duration) {
    QString definition = duration;
    definition.toLower();
    QHash<QString, double> values;
    values["y"] = -4713;
    values["m"] = 1.0;
    values["d"] = 1.0;
    values["h"] = 0.0;
    values["min"] = 0.0;
    values["s"] = 0.0;

    QString v;
    double rest = 0;
    bool timePart = false;
    for(int i=0; i < definition.size(); ++i) {
        char c = definition[i].toLatin1();
        if ( (c >= QLatin1Char('0') && c <= '9') || c == '.')
            v+=c;
        else if ( c == 'y' || c == 'd' || c == 'h' || c == 's') {
            double value = v.toDouble();
            values[QString(c)] += value;
            v = "";
        }
        else if ( c == 't') {
            timePart = true;
            v = "";
        } else if ( c == 'm' && timePart ) {
            values["min"] += v.toDouble();
            v = "";
        }
        else if ( c == 'm') {
            values["m"] += v.toDouble();
            v = "";
        }
    }
    int year, month, day, hour, minute;
    double seconds;
    year = values["y"];
    seconds = values["s"];
    minute = values["min"];
    hour = values["h"];
    day = values["d"];
    month = values["m"];

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

Duration::Duration(double r) : Time(r) {
    if ( r == rUNDEF || r == tUNDEF || r == 0)
        _valid = false;
    _valid = true;
}

QString Duration::toString(bool local, Time::Mode ) const{
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
TimeInterval::TimeInterval() {
    _step = Duration(tUNDEF);
}

TimeInterval::TimeInterval(const Time& beg, const Time& en, const Duration& stp) :
NumericRange((double)beg, (double)en),
_step(stp)
{
    if ( (double)beg == BIGTIME) {
        min(-BIGTIME);
    }
}

TimeInterval TimeInterval::operator+(const TimeInterval& interval){
    if ( _step == interval._step ) {
        return TimeInterval(Time(min() + interval.min()), Time(max() + interval.max(), _step));
    }
    else if (  (double)interval.getStep() == tUNDEF || _step == tUNDEF) {
        return TimeInterval(Time(min() + interval.min()), Time(max() + interval.max(), _step == tUNDEF ? interval.getStep() : _step));
    }
    return TimeInterval();
}
TimeInterval TimeInterval::operator-(const TimeInterval& interval){
        if ( _step == interval._step ) {
        return TimeInterval(Time(min() - interval.min()), Time(max() - interval.max(), _step));
    }
    else if (  interval.getStep() == tUNDEF || _step == tUNDEF) {
        return TimeInterval(Time(min() - interval.min()), Time(max() - interval.max(), _step == tUNDEF ? interval.getStep() : _step));
    }
    return TimeInterval();
}

QString TimeInterval::toString(bool local, Time::Mode mode) {
    Time begin(min());
    Time end(max());
    QString sb = begin.toString(local,mode);
    QString se = end.toString(local,mode);
    return QString("%1/%2").arg(sb ,se);
}

bool TimeInterval::contains(const QString &value, bool inclusive) const
{
    Time t(value);
    if ( !t.isValid())
        return false;
    return NumericRange::contains(t, inclusive);

}

bool TimeInterval::contains(const Time &value, bool inclusive) const
{
    return NumericRange::contains(value, inclusive);
}

QString TimeInterval::value(const QVariant &v) const
{
    QString type = v.typeName();
    if ( type != "Ilwis::Time"){
        ERROR2(ERR_COULD_NOT_CONVERT_2,v.toString(), "time");
        return sUNDEF;
    }
    Time t = v.value<Ilwis::Time>();
    return t.toString();
}


TimeInterval& TimeInterval::operator=(const TimeInterval& tiv){
    min(tiv.min());
    max(tiv.max());
    _step = tiv.getStep();

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









