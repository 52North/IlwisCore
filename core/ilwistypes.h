#ifndef ILWISTYPES_H
#define ILWISTYPES_H

#include "kernel_global.h"

typedef quint64 IlwisTypes;

const quint64 itUNKNOWN = 0;                           //0
const quint64 itPOINT =  1;                            //1
const quint64 itLINE =  2 * itPOINT;                   //2
const quint64 itPOLYGON = 2 * itLINE;                  //4
const quint64 itRASTER = 2 * itPOLYGON;                //8
const quint64 itNUMERICDOMAIN =  2 * itRASTER;         //16
const quint64 itITEMDOMAIN = 2 * itNUMERICDOMAIN;      //32
const quint64 itCOORDDOMAIN = 2 * itITEMDOMAIN ;       //64
const quint64 itCOLORDOMAIN = 2 * itCOORDDOMAIN ;      //128
const quint64 itTEXTDOMAIN = 2 * itCOLORDOMAIN ;       //256
const quint64 itCONVENTIONALCOORDSYSTEM = 2 * itTEXTDOMAIN; //512
const quint64 itORTHOCOORDSYSTEM = 2 * itCONVENTIONALCOORDSYSTEM; //1024
const quint64 itBOUNDSONLYCSY = 2 * itORTHOCOORDSYSTEM;//2048
const quint64 itELLIPSOID = 2  * itBOUNDSONLYCSY;      //4096
const quint64 itPROJECTION =  2 * itELLIPSOID;         //8192
const quint64 itATTRIBUTETABLE = 2 * itPROJECTION;     //16384
const quint64 itFLATTABLE = 2 * itATTRIBUTETABLE;      //32768
const quint64 itREPRESENTATION = 2 * itFLATTABLE;      //65536
const quint64 itGEOREF = 2 * itREPRESENTATION;         //131072
const quint64 itSINGLEOPERATION = 2 * itGEOREF;        //262144
const quint64 itCATALOG = 2 * itSINGLEOPERATION;       //524288
const quint64 itENVELOPE = 2 * itCATALOG;              //1048576
const quint64 itBOUNDINGBOX = 2 * itENVELOPE;          //2097152
const quint64 itRASTERSIZE = 2 * itBOUNDINGBOX;        //4194304
const quint64 itGEODETICDATUM = 2  * itRASTERSIZE;     //8388608
const quint64 itBOOL = 2 * itGEODETICDATUM;            //16777216
const quint64 itINT8 = 2 * itBOOL;                     //33554432
const quint64 itUINT8 = 2 * itINT8;                    //67108864
const quint64 itUINT16 =  2 * itUINT8;                 //134217728
const quint64 itINT16 =  2 * itUINT16;                 //268435456
const quint64 itUINT32 = 2 * itINT16;                  //536870912
const quint64 itINT32 = 2 * itUINT32;                  //1073741824
const quint64 itUINT64 = 2 * itINT32;                  //2147483648
const quint64 itINT64 = 2 * itUINT64;                  //4294967296
const quint64 itFLOAT = 2 * itINT64;                   //8589934592
const quint64 itDOUBLE = 2 * itFLOAT;                  //17179869184
const quint64 itVALUERANGE = 2 * itDOUBLE;             //34359738368
const quint64 itSTRING = 2 * itVALUERANGE;             //68719476736
const quint64 itDATE = 2 * itSTRING;                   //137438953472
const quint64 itTIME = 2 * itDATE;                     //274877906944
const quint64 itMETRICCOORDINATE = 2 * itTIME;         //549755813888
const quint64 itPIXEL = 2 *itMETRICCOORDINATE;         //1099511627776
const quint64 itBINARY = 2 * itPIXEL;                  //2199023255552
const quint64 itFILE = 2 * itBINARY;                   //4398046511104
const quint64 itURL = 2 * itFILE;                      //8796093022208
const quint64 itTHEMATICITEM = 2 * itURL;              //17592186044416
const quint64 itNAMEDITEM = 2 * itTHEMATICITEM;        //35184372088832
const quint64 itINDEXEDITEM = 2 * itNAMEDITEM;         //70368744177664
const quint64 itNUMERICITEM = 2 * itINDEXEDITEM;       //140737488355328
const quint64 itTIMEITEM = 2 * itNUMERICITEM;          //281474976710656
const quint64 itPALETTECOLOR = 2 * itTIMEITEM;         //562949953421312
const quint64 itCONTINUOUSCOLOR = 2 * itPALETTECOLOR;  //1125899906842624
const quint64 itSAMPLESET = 2 * itCONTINUOUSCOLOR;     //2251799813685248
const quint64 itCATALOGVIEW = 2 * itSAMPLESET;         //4503599627370496
const quint64 itCOLLECTION = 2 * itCATALOGVIEW;        //9007199254740992
const quint64 itARRAY = 2 * itCOLLECTION;              //18014398509481984
const quint64 itWORKFLOW = 2 * itARRAY;                //36028797018963968
const quint64 itWORKSPACE = 2 * itWORKFLOW;            //72057594037927936
const quint64 itLATLON = 2 * itWORKSPACE;               //144115188075855872
const quint64 itCOLUMN = 2 * itLATLON;                //288230376151711744

const quint64 itCOVERAGE = itPOINT | itRASTER | itPOLYGON | itLINE ; //15
const quint64 itFEATURE = itPOINT | itPOLYGON | itLINE; //7
const quint64 itCOORDSYSTEM = itCONVENTIONALCOORDSYSTEM | itORTHOCOORDSYSTEM | itBOUNDSONLYCSY; //7168
const quint64 itTABLE = itFLATTABLE | itATTRIBUTETABLE; //49152
const quint64 itOPERATIONMETADATA = itSINGLEOPERATION | itWORKFLOW;
const quint64 itDOMAIN =  itNUMERICDOMAIN | itITEMDOMAIN | itCOORDDOMAIN | itCOLORDOMAIN | itTEXTDOMAIN; //496
const quint64 itILWISOBJECT = itCOVERAGE | itDOMAIN | itCOORDSYSTEM | itCATALOG |
         itELLIPSOID | itPROJECTION | itTABLE | itREPRESENTATION | itGEOREF | itOPERATIONMETADATA;
const quint64 itDATETIME = itDATE | itTIME;
const quint64 itINTEGER = itINT8 | itUINT8 | itINT16 | itUINT16 | itINT32 | itINT64 | itUINT32 | itUINT64;
const quint64 itPOSITIVEINTEGER = itUINT8 | itUINT16 | itUINT32 | itUINT64;
const quint64 itNUMBER = itINTEGER | itFLOAT | itDOUBLE | itDATETIME;
const quint64 itNUMERIC = itNUMBER | itNUMERICITEM | itTIMEITEM;
const quint64 itDOMAINITEM = itTHEMATICITEM | itNAMEDITEM | itINDEXEDITEM | itNUMERICITEM | itTIMEITEM | itPALETTECOLOR;
const quint64 itIDENTIFIERITEM = itNAMEDITEM | itINDEXEDITEM;
const quint64 itCOORDINATE = itMETRICCOORDINATE | itLATLON;

const quint64 itRESOURCELOCATION = itFILE | itURL;
const quint64 itLOCATION = itCOORDINATE | itPIXEL;
const quint64 itCOLOR = itCONTINUOUSCOLOR | itPALETTECOLOR;

const quint64 itANY = 0xFFFFFFFFFFFFFFFF;

#define hasType(a,b) ( ((a) & (b)) != 0)

namespace Ilwis {
class KERNELSHARED_EXPORT TypeHelper {
public:
    static QString type2HumanReadable(IlwisTypes type);

    static QString type2name(IlwisTypes t);
};
}

#endif // ILWISTYPES_H
