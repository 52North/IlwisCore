#include <QString>
#include "ilwistypes.h"

using namespace Ilwis;

QString TypeHelper::type2HumanReadable(IlwisTypes t)
{
    switch(t) {
    case  itRASTER:
        return "Raster Coverage";
    case  itPOLYGON:
        return "Polygon Coverage";
    case  itLINE:
        return "Line Coverage";
    case  itPOINT:
        return "Point Coverage";
    case  itPOINT+itLINE:
        return "Feature Coverage";
    case  itPOINT+itPOLYGON:
        return "Feature Coverage";
    case  itPOLYGON+itLINE:
        return "Feature Coverage";
    case  itNUMERICDOMAIN:
        return "Value Domain";
    case  itITEMDOMAIN:
        return "Item Domain";
    case  itCOLORDOMAIN:
        return "Color Domain";
    case  itCOORDSYSTEM:
        return "Coordinate System";
    case  itCONVENTIONALCOORDSYSTEM:
        return "Conventional CoordinateSystem";
    case itBOUNDSONLYCSY:
        return "BoundsOnly CoordinateSystem"        ;
    case  itGEOREF:
        return "Georeference";
    case  itTABLE:
        return "Table";
    case  itPROJECTION:
        return "Projection";
    case  itELLIPSOID:
        return "Ellipsoid";
    case  itGEODETICDATUM:
        return "Geodetic Datum";
    case  itCATALOG:
        return "Catalog";
    case  itOPERATIONMETADATA:
        return "OperationMetaData";
        // nonilwisobjects
    case itENVELOPE:
        return "Envelope";
    case itBOOL:
        return "Boolean";
    case itBOUNDINGBOX:
        return "BoundingBox";
    case itRASTERSIZE:
        return "Raster Size";
    case itINT8:
        return "Signed Byte";
    case itUINT8:
        return "Byte";
    case itINT16:
        return "Integer 16-bit";
    case itUINT16:
        return "Positive 16-bit Integer";
    case itINT32:
        return "Integer 32-bit";
    case itUINT32:
        return "Positive 32-bit Integer";
    case itINT64:
        return "Integer 64-bit";
    case itUINT64:
        return "Positive 64-bit Integer";
    case itFLOAT:
        return "Floatingpoint 32-bit"        ;
    case itDOUBLE:
        return "Floatingpoint 64-bit";
    case itVALUERANGE:
        return "ValueRange";
    case itSTRING:
        return "String";
    case itDATE:
        return "Date";
    case itTIME:
        return "Time";
    case itCOORDINATE:
        return "Coordinate";
    case itPIXEL:
        return "Pixel";
    case itBINARY:
        return "Binary";
    case itFILE:
        return "File";
    case itURL:
        return "Url";
    case itTHEMATICITEM:
        return "Thematic class";
    case itNAMEDITEM:
        return "Named Identifier";
    case itINDEXEDITEM:
        return "Indexed Identifier";
    case itNUMERICITEM:
        return "Numeric Interval";
    case itTIMEITEM:
        return "Time Interval";
    case itPALETTECOLOR:
        return "Palette Color";
    case itCONTINUOUSCOLOR:
        return "Color";
    case itSAMPLESET:
        return "Sample Set";
    case itCATALOGVIEW:
        return "CatalogView";
    case itCOLLECTION:
        return "Collection";
    case itBUFFER:
        return "Buffer";
    case itCOVERAGE:
        return "Coverage";
    case itFEATURE:
        return "Feature";
    case itDOMAIN:
        return "Domain";
    case itDATETIME:
        return "DateTime";
    case itINTEGER:
        return "Integer";
    case itPOSITIVEINTEGER:
        return "Positive Integer";\
    case itNUMBER:
        return "Number";
    case itDOMAINITEM:
        return "Domain Item";
    case itLOCATION:
        return "location";
    }
    return "?";

}
