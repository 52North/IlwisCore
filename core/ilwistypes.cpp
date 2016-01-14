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
    case  itCOLUMN:
        return TR("Column");
    }
    return TR("?");

}

QString TypeHelper::type2name(IlwisTypes t)
{
    switch(t) {
    case  itRASTER:
        return "rastercoverage";
    case  itPOLYGON:
        return "polygoncoverage";
    case  itLINE:
        return "linecoverage";
    case  itPOINT:
        return "pointcoverage";
    case  itPOINT+itLINE:
        return "featurecoverage";
    case  itPOINT+itPOLYGON:
        return "featurecoverage";
    case  itPOLYGON+itLINE:
        return "featurecoverage";
    case  itNUMERICDOMAIN:
        return "numericdomain";
    case  itITEMDOMAIN:
        return "itemdomain";
    case  itCOLORDOMAIN:
        return "colordomain";
    case  itTEXTDOMAIN:
        return "textdomain";
    case  itCOORDSYSTEM:
        return "coordinatesystem";
    case  itCONVENTIONALCOORDSYSTEM:
        return "conventionalcoordinatesystem";
    case itBOUNDSONLYCSY:
        return "boundsonlycoordinatesystem";
    case  itGEOREF:
        return "georeference";
    case  itTABLE:
    case itFLATTABLE:
        return "table";
    case  itPROJECTION:
        return "projection";
    case  itELLIPSOID:
        return "ellipsoid";
    case  itGEODETICDATUM:
        return "geodeticdatum";
    case  itCATALOG:
        return "catalog";
    case  itWORKFLOW:
        return "workflow";
    case  itSINGLEOPERATION:
        return "singleoperation";
    case  itOPERATIONMETADATA:
        return "operationmetatadata";
    case itREPRESENTATION:
        return "representation";
        // nonilwisobjects
    case itENVELOPE:
        return "envelope";
    case itBOOL:
        return "boolean";
    case itBOUNDINGBOX:
        return "boundingbox";
    case itRASTERSIZE:
        return "rastersize";
    case itINT8:
        return "signedbyte";
    case itUINT8:
        return "byte";
    case itINT16:
        return "int16";
    case itUINT16:
        return "uint16";
    case itINT32:
        return "int32";
    case itUINT32:
        return "uint32";
    case itINT64:
        return "int64";
    case itUINT64:
        return "uint64";
    case itFLOAT:
        return "real32";
    case itDOUBLE:
        return "real64";
    case itVALUERANGE:
        return "valuerange";
    case itSTRING:
        return "sing";
    case itDATE:
        return "date";
    case itTIME:
        return "time";
    case itCOORDINATE:
        return "coordinate";
    case itPIXEL:
        return "pixel";
    case itBINARY:
        return "binary";
    case itFILE:
        return "file";
    case itURL:
        return "url";
    case itTHEMATICITEM:
        return "thematicclass";
    case itNAMEDITEM:
        return "namedidentifier";
    case itINDEXEDITEM:
        return "indexidentifier";
    case itNUMERICITEM:
        return "numericinterval";
    case itTIMEITEM:
        return "timeinterval";
    case itPALETTECOLOR:
        return "palettecolor";
    case itCONTINUOUSCOLOR:
        return "color";
    case itSAMPLESET:
        return "sampleset";
    case itCATALOGVIEW:
        return "catalogview";
    case itCOLLECTION:
        return "collection";
    case itARRAY:
        return "array";
    case itCOVERAGE:
        return "coverage";
    case itFEATURE:
        return "featurecoverage";
    case itDOMAIN:
        return "domain";
    case itDATETIME:
        return "datetime";
    case itINTEGER:
        return "int";
    case itPOSITIVEINTEGER:
        return "uint";\
    case itNUMBER:
        return "number";
    case itDOMAINITEM:
        return "domainitem";
    case itLOCATION:
        return "location";
    case itMETRICCOORDINATE:
        return "meiccoordinate";
    case itLATLON:
        return "latlon";
    case itCOLUMN:
        return "column"        ;
    }
    return sUNDEF;

}
