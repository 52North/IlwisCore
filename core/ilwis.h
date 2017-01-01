#ifndef ILWIS_H
#define ILWIS_H

/*! \mainpage Ilwis-Objects framework
 *
* \section intro_sec Introduction
*
*   The objective of the ILWIS NG is to create a framework that allows a seamless integration between heterogeneous data sources and processing sources. The framework will allow us to create applications that are largely independent of the format/nature of the underlying data containers or processing sources. Particularly we are thinking about:
*
*   - Scripting environment for accessing and manipulating GIS data containers in a more “database” like way (probably anchored in Python).
*   - Desktop environment for integrating remote and local sources of data and processing
*   - Geo - Processing engine in a web server environment.
*   - Exploration of P2P, distributed processing, OpenCL/Cuda.
*
* The secondary objective is to create a UI toolbox to facilitate an easy way to create a user interface for applications using ILWIS NG . Since the advent of mobile devices the concepts of UI design has seen a lot of movement after being fairly static for the last 10 years. This is an interesting area of exploration.
*/

#include <QVariant>
#include <limits>
#include <float.h>
#include <math.h>
#include <functional>
#include <memory>
#include "errmessages.h"

#define VALID(a) (a.isValid() && a->isValid())

namespace Ilwis {
//undefs

typedef unsigned char byte;

const quint8  bUNDEF = 2;
const short  shUNDEF = std::numeric_limits < qint16 >::max() - 2;
const short  shILLEGAL = shUNDEF + 1;
const long   iUNDEF  = std::numeric_limits < qint32 >::max() - 2;
const long   iILLEGAL = iUNDEF + 1;
const double rUNDEF = -1e308;
const double rILLEGAL = rUNDEF + 1;
const float flUNDEF = ((float)std::numeric_limits < float >::max() - 2);
const qint64 i64UNDEF = std::numeric_limits < qint64 >::max() - 2;
const double clrUNDEF2 = (std::numeric_limits < quint32 >::max() + 1);

#define sUNDEF "?"


inline double min(double a, double b) { return ((a<=b && a!=rUNDEF && a!= iUNDEF)  || b==rUNDEF) ? a : b; }
inline double max(double a, double b) { return (a>=b && a!=rUNDEF && a != iUNDEF) ? a : b; }
inline long min(long a, long b) { return ((a<=b && a!=iUNDEF) || b==iUNDEF) ? a : b; }
inline long max(long a, long b) { return (a>=b && a!=iUNDEF) ? a : b; }
inline qint64 max(qint64 a, qint64 b) { return a>=b && a!=i64UNDEF ? a : b;}

enum LogicalOperator{loNONE,loAND, loOR, loXOR, loLESS, loLESSEQ, loNEQ, loEQ, loGREATER, loGREATEREQ, loNOT};

}

#include "ilwistypes.h"

//#define M_LN10      2.30258509299404568402
#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif
//#define M_PI_2      1.57079632679489661923
//#define M_PI_4      0.785398163397448309616
#define M_1_SQRTPI  0.564189583547756286948
//#define M_2_SQRTPI  1.12837916709551257390
//#define M_SQRT2     1.41421356237309504880
#define M_SQRT_2    0.707106781186547524401
#define M_RAD_BASE  57.29577951308232087502

const double EPS15=1.e-15;
const double EPS12=1.e-12;
const double EPS10=1.e-10;
const double EPS8=1.e-8;
const double EPS7=1.e-7;
const double EPS6=1.e-6;
const double EPS5=1.e-5;

const int COVERAGEATRIB = 100000000;
const int STREAMBLOCKSIZE = 10e6;
const QString PIXELVALUE = "Pixel value";

template<class T> inline IlwisTypes numericType() {
    bool isSigned = std::numeric_limits<T>::is_signed;
    if ( std::numeric_limits<T>::is_integer) {
        T m2 = std::numeric_limits<T>::max();
        if ( m2 < 256)
            return isSigned ? itINT8 : itUINT8;
        else if ( m2 < 65536 )
            return isSigned ? itINT16 : itUINT16;
        else if ( m2 <= 4294967296L)
            return isSigned ? itINT32 : itUINT32;
        else if ( m2 <= qint64(9223372036854775807))
            return (T)(isSigned ? itINT64 : itUINT64);
    } else {
        double m2 = std::numeric_limits<T>::max();
        if ( m2 == std::numeric_limits<double>::max()){
            return itDOUBLE;
        } else if ( m2 == std::numeric_limits<float>::max())
            return itFLOAT;
    }
    return itUNKNOWN;
}

template<class T> T undef() {
    IlwisTypes type = numericType<T>();
    switch(type) {
    case itINT8:
    case itUINT8:
        return 0;
    case itINT16:
    case itUINT16:
        return (T)Ilwis::shUNDEF;
    case itINT32:
    case itUINT32:
        return (T)Ilwis::iUNDEF;
    case itFLOAT:
        return (T)Ilwis::flUNDEF;
    case itDOUBLE:
        return (T)Ilwis::rUNDEF;
    case itINT64:
        return (T)Ilwis::i64UNDEF;
    default:
        break;
    }
    return T();
}

typedef double Raw; // originally raws were integers and they still are but as the undefineds are doubles; they must fit in there



#define isNumericalUndef(v) (v == Ilwis::rUNDEF || v == Ilwis::iUNDEF || v == Ilwis::shUNDEF || v == Ilwis::flUNDEF)
#define isNumericalUndef2(v,obj) (v == Ilwis::rUNDEF || v == Ilwis::iUNDEF || v == Ilwis::shUNDEF || v == Ilwis::flUNDEF || v == obj->pseudoUndef())

//round
template<typename T> T round(T x, T frac) {
    if ( isNumericalUndef(x) || frac == 0)
        return x;
    T numvalue = (std::ceil(x / frac)) * frac;
    return numvalue;
}

#endif // ILWIS_H
