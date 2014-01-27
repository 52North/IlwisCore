#ifndef ANGLE_H
#define ANGLE_H


#include <sstream>
#include <iomanip>
#include "ilwis.h"

namespace Ilwis {
class Angle {
public:
    enum Unit{uRADIANS, uDEGREES};

    Angle() : _radians(rUNDEF), _behaveAsRadians(true){}
    Angle(double angle, bool asRadians=true) {
        _behaveAsRadians = asRadians;
        if ( asRadians)
            _radians = angle;
        else
            _radians = angle / M_RAD_BASE;
    }
    bool isValid() const {
        return _radians >= 0;
    }

    double degrees() const {
        return _radians * M_RAD_BASE;
    }

    double radians() const {
        return _radians;
    }
    operator double() const {
        return convertToDouble();
    }

    operator std::string() const {
        std::stringstream txt;
        if ( _behaveAsRadians) {
            txt << std::fixed << std::setprecision(8) << _radians;
            return  txt.str();
        }
        int dgr = (int)_radians;
        int minutes = (_radians - dgr) * 60;
        double sec = (( _radians - dgr) - minutes/60.0) * 3600.0;
        txt << dgr << " " << minutes <<"' " << std::fixed << std::setprecision(3) << sec;
        return txt.str();

    }

    Angle& operator+=(const Angle& a) {
        _radians += a.radians();
        _radians = fmod(_radians ,2 * M_PI);
        return *this;
    }

    Angle& operator*=(double f) {
        _radians *= f;
        _radians = fmod(_radians ,2 * M_PI);
        _radians = _radians > 0 ? _radians : 2 * M_PI - _radians;
        return *this;
    }

    Angle& operator/=(double f) {
        if ( f == 0)
            return *this;
        _radians /= f;
        _radians = fmod(_radians ,2 * M_PI);
        _radians = _radians > 0 ? _radians : 2 * M_PI - _radians;
        return *this;
    }

    Angle& operator-=(const Angle& a) {
        _radians -= a.radians();
        _radians = fmod(_radians ,2 * M_PI);
        _radians = _radians > 0 ? _radians : 2 * M_PI - _radians;
        return *this;
    }

    bool asRadians() const {
        return _behaveAsRadians;
    }

    virtual void asRadians(bool yesno) {
        _behaveAsRadians = yesno;
    }

protected:
    double _radians;
    bool _behaveAsRadians;

    virtual double convertToDouble() const {
        if ( _behaveAsRadians )
            return radians();
        return degrees();
    }
};

class Radians : public Angle{
public:
    Radians() : Angle() {}
    Radians(double v, bool isDegrees=false) : Angle(v, !isDegrees ) {}
    Radians(const Angle& r) { _radians = r.radians(); _behaveAsRadians = true;}
    Radians& operator=(const Angle& r) {
        _radians = r.radians();
        _behaveAsRadians = true;
        return *this;
    }
private:
    virtual void asRadians(bool ) {}
};

class Degrees : public Angle{
public:
    Degrees() : Angle() {}
    Degrees(double v, bool isDegrees=true) : Angle(v, !isDegrees) {}
    Degrees(const Angle& r) { _radians = r.radians(); _behaveAsRadians = false;}
    Degrees& operator=(const Angle& r) {
        _radians = r.radians();
        _behaveAsRadians = false;
        return *this;
    }
private:
   virtual void asRadians(bool ) {}
};
}




#endif // ANGLE_H
