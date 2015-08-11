#include <QString>
#include "kernel.h"
#include "ilwistypes.h"

using namespace Ilwis;

QString TypeHelper::type2HumanReadable(IlwisTypes t)
{
    switch(t) {
    case  itRASTER:
        return TR("Raster Coverage");
    case  itPOLYGON:
        return TR("Polygon Coverage");
    case  itLINE:
        return TR("Line Coverage");
    case  itPOINT:
        return TR("Point Coverage");
    case  itPOINT+itLINE:
        return TR("Feature Coverage");
    case  itPOINT+itPOLYGON:
        return TR("Feature Coverage");
    case  itPOLYGON+itLINE:
        return TR("Feature Coverage");
    case  itNUMERICDOMAIN:
        return TR("Value Domain");
    case itTEXTDOMAIN:
        return TR("Text Domain");
    case  itITEMDOMAIN:
        return TR("Item Domain");
    case  itCOLORDOMAIN:
        return TR("Color Domain");
    case  itCOORDSYSTEM:
        return TR("Coordinate System");
    case  itCONVENTIONALCOORDSYSTEM:
        return TR("Conventional CoordinateSystem");
    case itBOUNDSONLYCSY:
        return TR("BoundsOnly CoordinateSystem");
    case  itGEOREF:
        return TR("Georeference");
    case  itTABLE:
        return TR("Table");
    case  itPROJECTION:
        return TR("Projection");
    case  itELLIPSOID:
        return TR("Ellipsoid");
    case  itGEODETICDATUM:
        return TR("Geodetic Datum");
    case  itCATALOG:
        return TR("Catalog");
    case  itWORKFLOW:
        return TR("Workflow Operation");
    case  itSINGLEOPERATION:
        return TR("Single Operation");
    case  itOPERATIONMETADATA:
        return TR("OperationMetaData");
    case  itREPRESENTATION:
        return TR("Representation");

        // nonilwisobjects
    case itENVELOPE:
        return TR("Envelope");
    case itBOOL:
        return TR("Boolean");
    case itBOUNDINGBOX:
        return TR("BoundingBox");
    case itRASTERSIZE:
        return TR("Raster Size");
    case itINT8:
        return TR("Signed Byte");
    case itUINT8:
        return TR("Byte");
    case itINT16:
        return TR("Integer 16-bit");
    case itUINT16:
        return TR("Positive 16-bit Integer");
    case itINT32:
        return TR("Integer 32-bit");
    case itUINT32:
        return TR("Positive 32-bit Integer");
    case itINT64:
        return TR("Integer 64-bit");
    case itUINT64:
        return TR("Positive 64-bit Integer");
    case itFLOAT:
        return TR("Floatingpoint 32-bit");
    case itDOUBLE:
        return TR("Floatingpoint 64-bit");
    case itVALUERANGE:
        return TR("ValueRange");
    case itSTRING:
        return TR("String");
    case itDATE:
        return TR("Date");
    case itTIME:
        return TR("Time");
    case itCOORDINATE:
        return TR("Coordinate");
    case itPIXEL:
        return TR("Pixel");
    case itBINARY:
        return TR("Binary");
    case itFILE:
        return TR("File");
    case itURL:
        return TR("Url");
    case itTHEMATICITEM:
        return TR("Thematic class");
    case itNAMEDITEM:
        return TR("Named Identifier");
    case itINDEXEDITEM:
        return TR("Indexed Identifier");
    case itNUMERICITEM:
        return TR("Numeric Interval");
    case itTIMEITEM:
        return TR("Time Interval");
    case itPALETTECOLOR:
        return TR("Palette Color");
    case itCONTINUOUSCOLOR:
        return TR("Color");
    case itSAMPLESET:
        return TR("Sample Set");
    case itCATALOGVIEW:
        return TR("CatalogView");
    case itCOLLECTION:
        return TR("Collection");
    case itARRAY:
        return TR("Array");
    case itCOVERAGE:
        return TR("Coverage");
    case itFEATURE:
        return TR("Feature");
    case itDOMAIN:
        return TR("Domain");
    case itDATETIME:
        return TR("DateTime");
    case itINTEGER:
        return TR("Integer");
    case itPOSITIVEINTEGER:
        return TR("Positive Integer");\
    case itNUMBER:
        return TR("Number");
    case itDOMAINITEM:
        return TR("Domain Item");
    case itLOCATION:
        return TR("location");
    }
    return TR("?");

}

QString TypeHelper::type2name(IlwisTypes t)
{
    switch(t) {
    case  itRASTER:
        return TR("rastercoverage");
    case  itPOLYGON:
        return TR("polygoncoverage");
    case  itLINE:
        return TR("linecoverage");
    case  itPOINT:
        return TR("pointcoverage");
    case  itPOINT+itLINE:
        return TR("featurecoverage");
    case  itPOINT+itPOLYGON:
        return TR("featurecoverage");
    case  itPOLYGON+itLINE:
        return TR("featurecoverage");
    case  itNUMERICDOMAIN:
        return TR("numericdomain");
    case  itITEMDOMAIN:
        return TR("itemdomain");
    case  itCOLORDOMAIN:
        return TR("colordomain");
    case  itTEXTDOMAIN:
        return TR("textdomain");
    case  itCOORDSYSTEM:
        return TR("coordinatesystem");
    case  itCONVENTIONALCOORDSYSTEM:
        return TR("conventionalcoordinatesystem");
    case itBOUNDSONLYCSY:
        return TR("boundsonlycoordinatesystem");
    case  itGEOREF:
        return TR("georeference");
    case  itTABLE:
        return TR("table");
    case  itPROJECTION:
        return TR("projection");
    case  itELLIPSOID:
        return TR("ellipsoid");
    case  itGEODETICDATUM:
        return TR("geodeticdatum");
    case  itCATALOG:
        return TR("catalog");
    case  itWORKFLOW:
        return TR("workflow");
    case  itSINGLEOPERATION:
        return TR("singleoperation");
    case  itOPERATIONMETADATA:
        return TR("operationmetatadata");
    case itREPRESENTATION:
        return TR("representation");
        // nonilwisobjects
    case itENVELOPE:
        return TR("envelope");
    case itBOOL:
        return TR("boolean");
    case itBOUNDINGBOX:
        return TR("boundingbox");
    case itRASTERSIZE:
        return TR("rastersize");
    case itINT8:
        return TR("signedbyte");
    case itUINT8:
        return TR("byte");
    case itINT16:
        return TR("int16");
    case itUINT16:
        return TR("uint16");
    case itINT32:
        return TR("Int32");
    case itUINT32:
        return TR("uint32");
    case itINT64:
        return TR("Int64");
    case itUINT64:
        return TR("uint64");
    case itFLOAT:
        return TR("Real32");
    case itDOUBLE:
        return TR("Real64");
    case itVALUERANGE:
        return TR("valuerange");
    case itSTRING:
        return TR("string");
    case itDATE:
        return TR("date");
    case itTIME:
        return TR("time");
    case itCOORDINATE:
        return TR("coordinate");
    case itPIXEL:
        return TR("pixel");
    case itBINARY:
        return TR("binary");
    case itFILE:
        return TR("file");
    case itURL:
        return TR("url");
    case itTHEMATICITEM:
        return TR("thematicclass");
    case itNAMEDITEM:
        return TR("namedidentifier");
    case itINDEXEDITEM:
        return TR("indexidentifier");
    case itNUMERICITEM:
        return TR("numericinterval");
    case itTIMEITEM:
        return TR("timeinterval");
    case itPALETTECOLOR:
        return TR("palettecolor");
    case itCONTINUOUSCOLOR:
        return TR("color");
    case itSAMPLESET:
        return TR("sampleset");
    case itCATALOGVIEW:
        return TR("catalogview");
    case itCOLLECTION:
        return TR("collection");
    case itARRAY:
        return TR("array");
    case itCOVERAGE:
        return TR("coverage");
    case itFEATURE:
        return TR("featurecoverage");
    case itDOMAIN:
        return TR("domain");
    case itDATETIME:
        return TR("datetime");
    case itINTEGER:
        return TR("int");
    case itPOSITIVEINTEGER:
        return TR("uint");\
    case itNUMBER:
        return TR("number");
    case itDOMAINITEM:
        return TR("domainitem");
    case itLOCATION:
        return TR("location");
    }
    return TR("?");

}
