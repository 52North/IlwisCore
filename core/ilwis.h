#ifndef ILWIS_H
#define ILWIS_H

#include <QVariant>
#include <limits>
#include <float.h>
#include <math.h>
#include <functional>
#include <memory>
#include "errmessages.h"

#define TR(s) (kernel()->translate(s))
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
#define crdUNDEF Coordinate(rUNDEF, rUNDEF, rUNDEF)
#define llUNDEF  LatLon(rUNDEF, rUNDEF)

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

typedef quint64 IlwisTypes;

const quint64 itUNKNOWN = 0; //0
const quint64 itPOINT =  1; //1
const quint64 itLINE =  2 * itPOINT; //2
const quint64 itPOLYGON = 2 * itLINE; //4
const quint64 itRASTER = 2 * itPOLYGON; //8
const quint64 itCOVERAGELIST = 2 * itRASTER; //16
const quint64 itOBJECTCOLLECTION =  2 * itCOVERAGELIST; //32
const quint64 itNUMERICDOMAIN =  2 * itOBJECTCOLLECTION; //64
const quint64 itITEMDOMAIN = 2 * itNUMERICDOMAIN; //128
const quint64 itTIMEDOMAIN = 2 * itITEMDOMAIN; //256
const quint64 itCOORDDOMAIN = 2 * itTIMEDOMAIN ; //512
const quint64 itCOLORDOMAIN = 2 * itCOORDDOMAIN ; //1024
const quint64 itTEXTDOMAIN = 2 * itCOLORDOMAIN ; //2048
const quint64 itCONVENTIONALCOORDSYSTEM = 2 * itTEXTDOMAIN; //4096
const quint64 itORTHOCOORDSYSTEM = 2 * itCONVENTIONALCOORDSYSTEM; //8192
const quint64 itELLIPSOID = 2  * itORTHOCOORDSYSTEM; // 16384
const quint64 itPROJECTION =  2 * itELLIPSOID; //32768
const quint64 itDATABASETABLE = 2 * itPROJECTION; //65536
const quint64 itFLATTABLE = 2 * itDATABASETABLE; //131072
const quint64 itREPRESENTATION = 2 * itFLATTABLE; //262144
const quint64 itGEOREF = 2 * itREPRESENTATION; //524288
const quint64 itENVELOPE = 2 * itGEOREF; //2097152
const quint64 itOPERATIONMETADATA = 2 * itENVELOPE; //4194304
const quint64 itRASTERSIZE = 2 * itOPERATIONMETADATA; //8388608
const quint64 itCATALOG = 2 * itRASTERSIZE;
const quint64 itCONTAINER = 2  * itCATALOG;
const quint64 itGEODETICDATUM = 2  * itCONTAINER;
const quint64 itBOOL = 2 * itGEODETICDATUM;
const quint64 itINT8 = 2 * itBOOL;
const quint64 itUINT8 = 2 * itINT8;
const quint64 itUINT16 =  2 * itUINT8;
const quint64 itINT16 =  2 * itUINT16;
const quint64 itUINT32 = 2 * itINT16;
const quint64 itINT32 = 2 * itUINT32;
const quint64 itUINT64 = 2 * itINT32;
const quint64 itINT64 = 2 * itUINT64;
const quint64 itFLOAT = 2 * itINT64;
const quint64 itDOUBLE = 2 * itFLOAT;
const quint64 itVALUERANGE = 2 * itDOUBLE;
const quint64 itSTRING = 2 * itVALUERANGE;
const quint64 itTIME = 2 * itSTRING;
const quint64 itCOORD2D = 2 * itTIME;
const quint64 itCOORD3D = 2 * itCOORD2D;
const quint64 itPIXEL = 2 *itCOORD3D;
const quint64 itVOXEL = 2 * itPIXEL;
const quint64 itBINARY = 2 * itVOXEL;
const quint64 itFILE = 2 * itBINARY;
const quint64 itURL = 2 * itFILE;
const quint64 itTHEMATICITEM = 2 * itFILE;
const quint64 itNAMEDITEM = 2 * itTHEMATICITEM;
const quint64 itINDEXEDITEM = 2 * itNAMEDITEM;
const quint64 itNUMERICITEM = 2 * itINDEXEDITEM;
const quint64 itTIMEITEM = 2 * itNUMERICITEM;
const quint64 itCOLOR = 2 * itTIMEITEM;


const quint64 itCOVERAGE = itPOINT | itRASTER | itPOLYGON | itLINE | itCOVERAGELIST;
const quint64 itFEATURE = itPOINT | itPOLYGON | itLINE;
const quint64 itCOORDSYSTEM = itCONVENTIONALCOORDSYSTEM | itORTHOCOORDSYSTEM;
const quint64 itTABLE = itFLATTABLE | itDATABASETABLE;
const quint64 itDOMAIN =  itNUMERICDOMAIN | itITEMDOMAIN | itTIMEDOMAIN | itCOORDDOMAIN | itCOLORDOMAIN | itTEXTDOMAIN;
const quint64 itILWISOBJECT = itCOVERAGE | itDOMAIN | itOBJECTCOLLECTION | itCOORDSYSTEM |
         itELLIPSOID | itPROJECTION | itTABLE | itREPRESENTATION | itGEOREF | itOPERATIONMETADATA;
const quint64 itINTEGER = itINT8 | itUINT8 | itINT16 | itUINT16 | itINT32 | itINT64 | itUINT32 | itUINT64;
const quint64 itNUMBER = itINTEGER | itFLOAT | itDOUBLE;
const quint64 itNUMERIC = itNUMBER | itNUMERICITEM;
const quint64 itDOMAINITEM = itTHEMATICITEM | itNAMEDITEM | itINDEXEDITEM | itNUMERICITEM | itTIMEITEM;
const quint64 itIDENTIFIERITEM = itNAMEDITEM | itINDEXEDITEM;

const quint64 itRESOURCELOCATION = itFILE | itURL;
const quint64 itCOORDINATE = itCOORD2D | itCOORD3D;
const quint64 itDISCRETECOORD = itPIXEL | itVOXEL;
const quint64 itLOCATION = itCOORDINATE | itDISCRETECOORD;

const quint64 itANY = 0xFFFFFFFFFFFFFFFF;

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
#define hasType(a,b) ( ((a) & (b)) != 0)


#endif // ILWIS_H
