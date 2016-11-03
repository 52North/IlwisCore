#ifndef ANGLE_H
#define ANGLE_H


#include <sstream>
#include <iomanip>
#include "ilwis.h"

namespace Ilwis {
class Angle {
public:
    enum Unit{uRADIANS, uDEGREES};

    Angle() : _degrees(rUNDEF){}
    Angle(double angle, bool isRadian=false) {
        if ( isRadian)
            _degrees = angle * M_RAD_BASE;
        else
            _degrees = angle ;
    }
    bool isValid() const {
        return _degrees >= 0;
    }

    double degrees() const {
        return _degrees ;
    }

    double radians() const {
        return _degrees / M_RAD_BASE;
    }
    operator double() const {
        return _degrees;
    }

    QString toString(bool asRadians) {
        std::stringstream txt;
        if ( asRadians) {
            txt << std::fixed << std::setprecision(8) << _degrees / M_RAD_BASE;
            return  QString(txt.str().c_str());
        }
        int dgr = (int)_degrees;
        int minutes = (_degrees - dgr) * 60;
        double sec = (( _degrees - dgr) - minutes/60.0) * 3600.0;
        txt << dgr << " " << minutes <<"' " << std::fixed << std::setprecision(3) << sec;
        return QString(txt.str().c_str());

    }

    Angle& operator+=(const Angle& a) {
        _degrees += a.degrees();
        _degrees = fmod(_degrees ,360.0);
        return *this;
    }

    Angle& operator*=(double f) {
        _degrees *= f;
        _degrees = fmod(_degrees ,360.0);
        _degrees = _degrees > 0 ? _degrees : 360.0 - _degrees;
        return *this;
    }

    Angle& operator/=(double f) {
        if ( f == 0)
            return *this;
        _degrees /= f;
        _degrees = fmod(_degrees ,360.0);
        _degrees = _degrees > 0 ? _degrees : 360.0 - _degrees;
        return *this;
    }

    Angle& operator-=(const Angle& a) {
        _degrees -= a.degrees();
        _degrees = fmod(_degrees ,360.0);
        _degrees = _degrees > 0 ? _degrees : 360.0 - _degrees;
        return *this;
    }

protected:
    double _degrees;


};

//class Radians : public Angle{
//public:
//    Radians() : Angle() {}
//    Radians(double v, bool isDegrees=false) : Angle(v, !isDegrees ) {}
//    Radians(const Angle& r) { _degrees = r.radians(); _behaveAsRadians = true;}
//    Radians& operator=(const Angle& r) {
//        _degrees = r.radians();
//        _behaveAsRadians = true;
//        return *this;
//    }
//private:
//    virtual void asRadians(bool ) {}
//};

//class Degrees : public Angle{
//public:
//    Degrees() : Angle() {}
//    Degrees(double v, bool isDegrees=true) : Angle(v, !isDegrees) {}
//    Degrees(const Angle& r) { _degrees = r.radians(); _behaveAsRadians = false;}
//    Degrees& operator=(const Angle& r) {
//        _degrees = r.radians();
//        _behaveAsRadians = false;
//        return *this;
//    }
//private:
//   virtual void asRadians(bool ) {}
//};
}




#endif // ANGLE_H
