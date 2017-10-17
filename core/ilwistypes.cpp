#include <QString>
#include "kernel.h"
#include "ilwisdata.h"
#include "ilwisobject.h"
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
    case itSCRIPT:
        return TR("Script");
    case itMODEL:
        return TR("Model");

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
    case  itCOMBINATIONMATRIX:
        return TR("Combination matrix");
    case itFLATTABLE:
        return "Table"        ;
    case itATTRIBUTETABLE:
        return "Attribute Table";
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
    case  itPOINT+itPOLYGON:
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
    case itSCRIPT:
        return "script";
    case itMODEL:
        return "model";
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
        return "string";
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
        return "integer";
    case itPOSITIVEINTEGER:
        return "uint";\
    case itNUMBER:
        return "number";
    case itDOMAINITEM:
        return "domainitem";
    case itLOCATION:
        return "location";
    case itMETRICCOORDINATE:
        return "metriccoordinate";
    case itLATLON:
        return "latlon";
    case itCOLUMN:
        return "column";
    case itCOMBINATIONMATRIX:
        return "combinationmatrix";
    }
//    QString type;
//    for(quint64 i =0; i < 64; ++i){
//       quint64 result = 1 << i;
//       if ( result > t)
//           break;

//        if ( hasType(t, result)) {
//            if ( type != "")
//                type += "|";
//            type += TypeHelper::type2name(result);
//        }

//    }
//    if ( type != "")
//        return type;

    return sUNDEF;

}

IlwisTypes TypeHelper::name2type(const QString &dname)
{

    QString name = dname.toLower();
    int index;
    if ( (index = name.indexOf("::")) != -1){
        name = dname.right(name.size() - index - 2);
    }

    if ( name =="ilwisobject")
        return  itILWISOBJECT;
    if ( name =="rastercoverage")
        return  itRASTER;
    if ( name == "polygoncoverage")
        return  itPOLYGON;
    if ( name == "linecoverage")
        return  itLINE;
    if ( name == "pointcoverage")
        return  itPOINT;
    if ( name == "featurecoverage")
        return  itFEATURE;
    if ( name == "coverage")
        return  itCOVERAGE;
    if ( name.mid(0,10) == "itemdomain") // contains template construct, so different comparison
        return  itITEMDOMAIN;
    if ( name == "numericdomain")
        return  itNUMERICDOMAIN;
    if ( name == "textdomain")
        return  itTEXTDOMAIN;
    if ( name == "colordomain")
        return  itCOLORDOMAIN;
    if ( name == "domain")
        return  itDOMAIN;
    if ( name == "coordinatesystem")
        return  itCOORDSYSTEM;
    if ( name == "conventionalcoordinatesystem")
        return  itCONVENTIONALCOORDSYSTEM;
    if ( name == "boundsonlycoordinatesystem")
        return  itBOUNDSONLYCSY;
    if ( name == "georeference")
        return  itGEOREF;
    if ( name == "table")
        return  itTABLE;
    if ( name == "flattable")
        return  itFLATTABLE;
    if ( name == "attributetable")
        return  itATTRIBUTETABLE;
    if ( name == "projection")
        return  itPROJECTION;
    if ( name == "ellipsoid")
        return  itELLIPSOID;
    if ( name == "geodeticdatum")
        return  itGEODETICDATUM;
    if ( name == "catalog")
        return  itCATALOG;
    if ( name == "singleoperation")
        return  itSINGLEOPERATION;
    if ( name == "workflow") {
        return  itWORKFLOW;
    }
    if ( name == "script") {
        return  itSCRIPT;
    }
    if ( name == "model") {
        return  itMODEL;
    }
    if ( name == "combinationmatrix") {
        return  itCOMBINATIONMATRIX;
    }
    if ( name == "operationmetaData") {
        return  itOPERATIONMETADATA;
    }
    if ( name == "catalog")
        return  itCATALOG;
    if ( name == "representation")
        return  itREPRESENTATION;
    if ( name == "column")
        return  itCOLUMN;
    if ( name == "string")
        return itSTRING;
    // standard c++ types
    if ( name == "int")
        return  itINT32;
    if ( name == "long")
        return  itINT32;
    if ( name == "unsigned int")
        return  itUINT32;
    if ( name == "unsigned long")
        return  itUINT32;
    if ( name == "short")
        return  itUINT16;
    if ( name == "unsigned short")
        return  itUINT16;
    if ( name == "char")
        return  itINT8;
    if ( name == "unsigned char")
        return  itUINT8;
    if ( name == "double")
        return  itDOUBLE;
    if ( name == "float")
        return  itFLOAT;
    if ( name == "bool")
        return  itBOOL;
    if ( name == "integer")
        return  itINTEGER;
    if ( name == "positiveinteger")
        return  itPOSITIVEINTEGER;
    if ( name == "char const *")
        return  itSTRING;
    if ( name == "char *")
        return  itSTRING;



    return itUNKNOWN;


}

IlwisTypes TypeHelper::variant2type(const QVariant &v)
{
    QVariant::Type ty = v.type();
    switch(ty) {
        case QVariant::Bool:
            return itUINT8; break;
        case QVariant::Double:
            return itDOUBLE; break;
        case QVariant::Int:
          return itINT32; break;
        case QVariant::LongLong:
        case QVariant::ULongLong:
            return itINT64; break;
        case QVariant::UInt:
            return itUINT32; break;
        case QVariant::String:
              return itSTRING; break;
        case QVariant::Time:
            return itTIME; break;
        case QVariant::Date:
            return itDATE; break;
        case QVariant::DateTime:
            return itDATETIME;
        case QVariant::List:
            return itCOLLECTION;
        default:
        {
            QString name = v.typeName();
            if ( name == "Ilwis::RasterCoverage" || name == "Ilwis::IRasterCoverage")
                return itRASTER;
            if ( name == "Ilwis::FeatureCoverage" || name == "IIlwis::FeatureCoverage")
                return itFEATURE;
            if ( name == "Ilwis::Table" || name == "Ilwis::ITable")
                return itTABLE;
            if ( name =="Ilwis::IlwisObject" || name =="Ilwis::IIlwisObject"){
                return  itILWISOBJECT;
            }
            if ( name == "Ilwis::Coverage" || name == "Ilwis::ICoverage")
                return  itCOVERAGE;
            if ( name.indexOf("Ilwis::ItemDomain")==0) // contains template construct, so different comparison
                return  itITEMDOMAIN;
            if ( name == "Ilwis::NumericDomain" ||  name == "Ilwis::INumericDomain")
                return  itNUMERICDOMAIN;
            if ( name == "Ilwis::TextDomain" || name == "Ilwis::ITextDomain")
                return  itTEXTDOMAIN;
            if ( name == "Ilwis::ColorDomain" || name == "IIlwis::IColorDomain")
                return  itCOLORDOMAIN;
            if ( name == "Ilwis::Domain" || name == "Ilwis::IDomain")
                return  itDOMAIN;
            if ( name == "Ilwis::CoordinateSystem" || name == "Ilwis::ICoordinateSystem")
                return  itCOORDSYSTEM;
            if ( name == "Ilwis::ConventionalCoordinateSystem" || name == "Ilwis::IConventionalCoordinateSystem")
                return  itCONVENTIONALCOORDSYSTEM;
            if ( name == "Ilwis::BoundsOnlyCoordinateSystem" || name == "Ilwis::IBoundsOnlyCoordinateSystem")
                return  itBOUNDSONLYCSY;
            if ( name == "Ilwis::GeoReference" || name == "Ilwis::IGeoReference")
                return  itGEOREF;
            if ( name == "Ilwis::FlatTable" || name == "Ilwis::IFlatTable")
                return  itFLATTABLE;
            if ( name == "Ilwis::AttributeTable" || name == "Ilwis::IAttributeTable")
                return  itATTRIBUTETABLE;
            if ( name == "Ilwis::Projection" || name == "Ilwis::IProjection")
                return  itPROJECTION;
            if ( name == "Ilwis::Ellipsoid" || name == "Ilwis::IEllipsoid")
                return  itELLIPSOID;
            if ( name == "Ilwis::Catalog" || name == "Ilwis::ICatalog")
                return  itCATALOG;
            if ( name == "Ilwis::SingleOperation" || name == "Ilwis::ISingleOperation")
                return  itSINGLEOPERATION;
            if ( name == "Ilwis::Workflow" || name == "Ilwis::IWorkflow") {
                return  itWORKFLOW;
            }
            if ( name == "Ilwis::Script" || name == "Ilwis::IScript") {
                return  itSCRIPT;
            }
            if ( name == "Ilwis::Model" || name == "Ilwis::IModel") {
                return  itMODEL;
            }
            if ( name == "Ilwis::CombinationMatrix" || name == "Ilwis::ICombinationMatrix") {
                return  itMODEL;
            }
            if ( name == "Ilwis::Representation" ||  name == "Ilwis::IRepresentation")
                return  itREPRESENTATION;
        }

    }
    return itUNKNOWN;
}
