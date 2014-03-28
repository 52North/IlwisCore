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
*The secondary objective is to create a UI toolbox to facilitate an easy way to create a user interface for applications using ILWIS NG . Since the advent of mobile devices the concepts of UI design has seen a lot of movement after being fairly static for the last 10 years. This is an interesting area of exploration.
 *
 * \section install_sec Installation
 *
 * \subsection step1 Step 1: Opening the box
 *
 * etc...
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
const short  shUNDEF = -32767;
const short  shILLEGAL = shUNDEF + 1;
const long   iUNDEF  = -2147483647L;
const long   iILLEGAL = iUNDEF + 1;
const double rUNDEF = -1e308;
const double rILLEGAL = rUNDEF + 1;
const float flUNDEF = ((float)-1e38);
const qint64 i64UNDEF = std::numeric_limits < qint64 >::min();

#define sUNDEF "?"

//round
inline long roundx(float x) {
    if ((x == flUNDEF) || (x > LONG_MAX) || (x < LONG_MIN))
        return iUNDEF;
    else
        return (long)floor(x + 0.5);
}
inline long roundx(double x) {
    if ((x == rUNDEF) || (x > LONG_MAX) || (x < LONG_MIN))
        return iUNDEF;
    else
        return (long)floor(x + 0.5);
}

//compare
inline byte   byteConv(short x)   { return x < 0 ? (byte)0 : x > 255 ? (byte)0 : (byte)x; }
inline byte   byteConv(long  x)   { return x < 0 ? (byte)0 : x > 255 ? (byte)0 : (byte)x; }
inline byte   byteConv(int x)     { return x < 0 ? (byte)0 : x > 255 ? (byte)0 : (byte)x; }
inline byte   byteConv(double x)  { return byteConv(roundx(x)); }
inline short  shortConv (long  x) { return  ((x == iUNDEF) || (x > SHRT_MAX) || (x < SHRT_MIN)) ? shUNDEF : (short)x; }
inline short  shortConv(double x) { return shortConv(roundx(x)); }
inline short  shortConv(short x)  { return x; }
inline short  shortConv(int x)    { return ((x == iUNDEF) || (x > SHRT_MAX) || (x < SHRT_MIN)) ? shUNDEF : (short)x;}
inline long   longConv(short x)   { return x == shUNDEF ? iUNDEF : (long)x; }
inline long   longConv(int x)     { return (long)x; }
inline long   longConv(float x)   { return roundx(x); }
inline long   longConv(double x)  { return roundx(x); }
inline float floatConv(short x)   { return x == shUNDEF ? flUNDEF : (float)x; }
inline float floatConv(long x)    { return x == iUNDEF ? flUNDEF : (float)x; }
inline float floatConv(double x)  { return ((x == rUNDEF) || (x < -FLT_MAX) || (x > FLT_MAX)) ? flUNDEF : (float)x; }
inline double doubleConv(short x) { return x == shUNDEF ? rUNDEF : (double)x; }
inline double doubleConv(long x)  { return x == iUNDEF ? rUNDEF : (double)x; }
inline double doubleConv(float x) { return x == flUNDEF ? rUNDEF : (double)x; }
inline double min(double a, double b) { return ((a<=b && a!=rUNDEF && a!= iUNDEF)  || b==rUNDEF) ? a : b; }
inline double max(double a, double b) { return (a>=b && a!=rUNDEF && a != iUNDEF) ? a : b; }
inline long min(long a, long b) { return ((a<=b && a!=iUNDEF) || b==iUNDEF) ? a : b; }
inline long max(long a, long b) { return (a>=b && a!=iUNDEF) ? a : b; }
inline qint64 max(qint64 a, qint64 b) { return a>=b && a!=i64UNDEF ? a : b;}

enum LogicalOperator{loNONE,loAND, loOR, loXOR, loLESS, loLESSEQ, loNEQ, loEQ, loGREATER, loGREATEREQ};
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


template<class T> inline IlwisTypes numericType() {
    bool isSigned = std::numeric_limits<T>::is_signed;
    if ( std::numeric_limits<T>::is_integer) {
        qint64 m2 = std::numeric_limits<T>::max();
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





#define isNumericalUndef(v) (v == Ilwis::rUNDEF || v == Ilwis::iUNDEF || v == Ilwis::shUNDEF || v == Ilwis::flUNDEF)

#endif // ILWIS_H
