#include "angle.h"

Ilwis::Angle operator+(const Ilwis::Angle& a1, const Ilwis::Angle& a2) {
    double sum = a1 + a2;
    Ilwis::Angle a(fmod(sum ,360.0));
    return a;
}

Ilwis::Angle operator*(const Ilwis::Angle& a1, double f) {
    double mul = a1 * f;
    return Ilwis::Angle(fmod(mul ,2 * M_PI));
}

Ilwis::Angle operator/(const Ilwis::Angle& a1, double f) {
    if ( f == 0)
        return Ilwis::Angle();
    double mul = a1 / f;
    return Ilwis::Angle(fmod(mul ,360.0));
}


Ilwis::Angle operator*( double f,const Ilwis::Angle& a1) {
    double mul = a1 * f;
    return Ilwis::Angle(fmod(mul ,360.0));
}

Ilwis::Angle operator-(const Ilwis::Angle& a1, const Ilwis::Angle& a2) {
    double sum = a1- a2;
    double res= fmod(sum ,360.0);
    res = res > 0 ? res : 360.0 - res;
    return Ilwis::Angle(res);
}
