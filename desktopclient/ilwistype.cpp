#include "kernel.h"
#include "ilwistype.h"

using namespace Ilwis;
using namespace Desktopclient;

quint64 IlwisType::unknown() const
{
    return itUNKNOWN;
}

quint64 IlwisType::point() const
{
    return itPOINT;
}

quint64 IlwisType::line() const
{
    return itLINE;
}

quint64 IlwisType::polygon() const
{
    return itPOLYGON;
}

quint64 IlwisType::grid() const
{
    return itRASTER;
}

quint64 IlwisType::numericdomain() const
{
    return itNUMERICDOMAIN;
}

quint64 IlwisType::itemdomain() const
{
    return itITEMDOMAIN;
}

quint64 IlwisType::coorddomain() const
{
    return itCOORDDOMAIN;
}

quint64 IlwisType::colordomain() const
{
    return itCOLORDOMAIN;
}

quint64 IlwisType::textdomain() const
{
    return itTEXTDOMAIN;
}

quint64 IlwisType::conventionalcoordsystem() const
{
    return itCONVENTIONALCOORDSYSTEM;
}

quint64 IlwisType::orthocoordsystem() const
{
    return itORTHOCOORDSYSTEM;
}

quint64 IlwisType::ellipsoid() const
{
    return itELLIPSOID;
}

quint64 IlwisType::projection() const
{
    return itPROJECTION;
}

quint64 IlwisType::attributetable() const
{
    return itATTRIBUTETABLE;
}

quint64 Ilwis::Desktopclient::IlwisType::flattable() const
{
    return itFLATTABLE;
}

quint64 IlwisType::georef() const
{
    return itGEOREF;
}

quint64 IlwisType::envelope() const
{
    return itENVELOPE;
}

quint64 IlwisType::operationmetadata() const
{
    return itOPERATIONMETADATA;
}


quint64 IlwisType::singleoperation() const
{
    return itSINGLEOPERATION;
}

quint64 IlwisType::workflow() const
{
    return itWORKFLOW;
}

quint64 IlwisType::gridsize() const
{
    return itRASTERSIZE;
}

quint64 IlwisType::catalog() const
{
    return itCATALOG;
}

quint64 IlwisType::geodeticdatum() const
{
    return itGEODETICDATUM;
}

quint64 IlwisType::boolean() const
{
    return itBOOL;
}

quint64 IlwisType::int8() const
{
    return itINT8;
}

quint64 IlwisType::uint8() const
{
    return itUINT8;
}

quint64 IlwisType::int16() const
{
    return itINT16;
}

quint64 IlwisType::uint16() const
{
    return itUINT16;
}

quint64 IlwisType::int32() const
{
    return itINT32;
}

quint64 IlwisType::uint32() const
{
    return itUINT32;
}

quint64 IlwisType::int64() const
{
    return itINT64;
}

quint64 IlwisType::real32() const
{
    return itFLOAT;
}

quint64 IlwisType::real64() const
{
    return itDOUBLE;
}

quint64 IlwisType::valuerange() const
{
    return itVALUERANGE;
}

quint64 IlwisType::string() const
{
    return itSTRING;
}

quint64 IlwisType::time() const
{
    return itTIME;
}

quint64 IlwisType::coordinate() const
{
    return itCOORDINATE;
}

quint64 IlwisType::pixel() const
{
    return itPIXEL;
}

quint64 IlwisType::binary() const
{
    return itBINARY;
}

quint64 IlwisType::file() const
{
    return itFILE;
}

quint64 IlwisType::url() const
{
    return itURL;
}

quint64 IlwisType::thematicitem() const
{
    return itTHEMATICITEM;
}

quint64 IlwisType::nameditem() const
{
    return itNAMEDITEM;
}

quint64 IlwisType::indexeditem() const
{
    return itINDEXEDITEM;
}

quint64 IlwisType::numericitem() const
{
    return itNUMERICITEM;
}

quint64 IlwisType::timeitem() const
{
    return itTIMEITEM;
}

quint64 IlwisType::color() const
{
    return itCOLOR;
}


quint64 IlwisType::uint64() const
{
    return itUINT64;
}

