#include "angle.h"

Ilwis::Angle operator+(const Ilwis::Angle& a1, const Ilwis::Angle& a2) {
    double sum = a1.radians() + a2.radians();
    Ilwis::Angle a(fmod(sum ,2 * M_PI));
    a.asRadians( a1.asRadians() || a2.asRadians() );
    return a;
}

Ilwis::Angle operator*(const Ilwis::Angle& a1, double f) {
    double mul = a1.radians() * f;
    return Ilwis::Angle(fmod(mul ,2 * M_PI));
}

Ilwis::Angle operator/(const Ilwis::Angle& a1, double f) {
    if ( f == 0)
        return Ilwis::Angle();
    double mul = a1.radians() / f;
    return Ilwis::Angle(fmod(mul ,2 * M_PI));
}


Ilwis::Angle operator*( double f,const Ilwis::Angle& a1) {
    double mul = a1.radians() * f;
    return Ilwis::Angle(fmod(mul ,2 * M_PI));
}

Ilwis::Angle operator-(const Ilwis::Angle& a1, const Ilwis::Angle& a2) {
    double sum = a1.radians() - a2.radians();
    double res= fmod(sum ,2 * M_PI);
    res = res > 0 ? res : 2 * M_PI - res;
    return Ilwis::Angle(res);
}
