#ifndef ILWISTYPES_H
#define ILWISTYPES_H

typedef quint64 IlwisTypes;

const quint64 itUNKNOWN = 0; //0
const quint64 itPOINT =  1; //1
const quint64 itLINE =  2 * itPOINT; //2
const quint64 itPOLYGON = 2 * itLINE; //4
const quint64 itRASTER = 2 * itPOLYGON; //8
const quint64 itNUMERICDOMAIN =  2 * itRASTER; //16
const quint64 itITEMDOMAIN = 2 * itNUMERICDOMAIN; //32
const quint64 itCOORDDOMAIN = 2 * itITEMDOMAIN ; //128
const quint64 itCOLORDOMAIN = 2 * itCOORDDOMAIN ; //256
const quint64 itTEXTDOMAIN = 2 * itCOLORDOMAIN ; //512
const quint64 itCONVENTIONALCOORDSYSTEM = 2 * itTEXTDOMAIN; //1024
const quint64 itORTHOCOORDSYSTEM = 2 * itCONVENTIONALCOORDSYSTEM; //2048
const quint64 itELLIPSOID = 2  * itORTHOCOORDSYSTEM; // 4096
const quint64 itPROJECTION =  2 * itELLIPSOID; //8192
const quint64 itDATABASETABLE = 2 * itPROJECTION;
const quint64 itFLATTABLE = 2 * itDATABASETABLE;
const quint64 itREPRESENTATION = 2 * itFLATTABLE;
const quint64 itGEOREF = 2 * itREPRESENTATION;
const quint64 itOPERATIONMETADATA = 2 * itGEOREF;
const quint64 itCATALOG = 2 * itOPERATIONMETADATA;
const quint64 itENVELOPE = 2 * itCATALOG;
const quint64 itRASTERSIZE = 2 * itENVELOPE;
const quint64 itGEODETICDATUM = 2  * itRASTERSIZE;
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
const quint64 itDATE = 2 * itSTRING;
const quint64 itTIME = 2 * itDATE;
const quint64 itCOORDINATE = 2 * itTIME;
const quint64 itPIXEL = 2 *itCOORDINATE;
const quint64 itBINARY = 2 * itPIXEL;
const quint64 itFILE = 2 * itBINARY;
const quint64 itURL = 2 * itFILE;
const quint64 itTHEMATICITEM = 2 * itFILE;
const quint64 itNAMEDITEM = 2 * itTHEMATICITEM;
const quint64 itINDEXEDITEM = 2 * itNAMEDITEM;
const quint64 itNUMERICITEM = 2 * itINDEXEDITEM;
const quint64 itTIMEITEM = 2 * itNUMERICITEM;
const quint64 itPALETTECOLOR = 2 * itTIMEITEM;
const quint64 itCONTINUOUSCOLOR = 2 * itPALETTECOLOR;
const quint64 itSAMPLESET = 2 * itCONTINUOUSCOLOR;
const quint64 itCATALOGVIEW = 2 * itSAMPLESET;
const quint64 itCOLLECTION = 2 * itCATALOGVIEW;

const quint64 itCOVERAGE = itPOINT | itRASTER | itPOLYGON | itLINE ;
const quint64 itFEATURE = itPOINT | itPOLYGON | itLINE;
const quint64 itCOORDSYSTEM = itCONVENTIONALCOORDSYSTEM | itORTHOCOORDSYSTEM;
const quint64 itTABLE = itFLATTABLE | itDATABASETABLE;
const quint64 itDOMAIN =  itNUMERICDOMAIN | itITEMDOMAIN | itCOORDDOMAIN | itCOLORDOMAIN | itTEXTDOMAIN;
const quint64 itILWISOBJECT = itCOVERAGE | itDOMAIN | itCOORDSYSTEM | itCATALOG |
         itELLIPSOID | itPROJECTION | itTABLE | itREPRESENTATION | itGEOREF | itOPERATIONMETADATA;
const quint64 itDATETIME = itDATE | itTIME;
const quint64 itINTEGER = itINT8 | itUINT8 | itINT16 | itUINT16 | itINT32 | itINT64 | itUINT32 | itUINT64;
const quint64 itNUMBER = itINTEGER | itFLOAT | itDOUBLE | itDATETIME;
const quint64 itNUMERIC = itNUMBER | itNUMERICITEM | itTIMEITEM;
const quint64 itDOMAINITEM = itTHEMATICITEM | itNAMEDITEM | itINDEXEDITEM | itNUMERICITEM | itTIMEITEM | itPALETTECOLOR;
const quint64 itIDENTIFIERITEM = itNAMEDITEM | itINDEXEDITEM;

const quint64 itRESOURCELOCATION = itFILE | itURL;
const quint64 itLOCATION = itCOORDINATE | itPIXEL;
const quint64 itCOLOR = itCONTINUOUSCOLOR | itPALETTECOLOR;

const quint64 itANY = 0xFFFFFFFFFFFFFFFF;

#define hasType(a,b) ( ((a) & (b)) != 0)

#endif // ILWISTYPES_H
