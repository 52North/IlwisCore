#include "kernel.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "datadefinition.h"
#include "module.h"
#include "ilwiscontext.h"
#include "catalog.h"
#include "numericdomain.h"
#include "numericrange.h"
#include "columndefinition.h"
#include "table.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "textdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "gdalproxy.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "dataformat.h"
#include "gdalconnector.h"
#include "juliantime.h"
#include "domainhelper.h"

#include "gdaltableloader.h"

using namespace Ilwis;
using namespace Gdal;

GdalTableLoader::GdalTableLoader(){
}

GdalTableLoader::~GdalTableLoader()
{
    for(FillerColumnDef *def : _columnFillers)
        delete def;
}

void GdalTableLoader::loadMetaData(Table *attTable, OGRLayerH hLayer) {
    int temp = gdal()->getFeatureCount(hLayer, FALSE);//TRUE to FORCE databases to scan whole layer, FALSe can end up in -1 for unknown result
    int recordCount = attTable->recordCount();
    recordCount = (temp == -1) ? 0 : temp;
    attTable->recordCount(recordCount);

    OGRFeatureDefnH hLayerDef = gdal()->getLayerDef(hLayer);
    int fieldCount = gdal()->getFieldCount(hLayerDef);
    for (int i = 0; i < fieldCount; i++){
        OGRFieldDefnH hFieldDefn = gdal()->getFieldDfn(hLayerDef, i);
        QString name = QString(gdal()->getFieldName(hFieldDefn));
        OGRFieldType type = gdal()->getFieldType(hFieldDefn);
        //OGR_Fld_GetWidth, OGR_Fld_GetPrecision, OGR_Fld_IsIgnored
        IDomain domain;
        switch(type){
        case OFTInteger:{ // Simple 32bit integer
            INumericDomain ndomain;
            ndomain.prepare("integer");
            domain = ndomain;break;
        }
        case OFTIntegerList:break; // List of 32bit integers

        case OFTReal:{ // Double Precision floating point
            INumericDomain ndomain;
            ndomain.prepare("value");
            domain = ndomain; break;
        }
        case OFTRealList:break; // List of doubles
        case OFTString:{ // String of ASCII chars
            domain.prepare("code=domain:text",itTEXTDOMAIN);
            break;
        }
        case OFTStringList: // Array of strings
        case OFTWideString:// deprecated
        case OFTWideStringList:// deprecated
        case OFTBinary: break; // Raw Binary data

        case OFTDate:{
            ITimeDomain tdomain;
            tdomain.prepare();
            tdomain->range(new TimeInterval(itDATE));
            domain = tdomain; break;
        }
        case OFTTime:{
            ITimeDomain tdomain;
            tdomain.prepare();
            tdomain->range(new TimeInterval(itTIME));
            domain = tdomain; break;
        }
        case OFTDateTime:{  // Date and Time
            ITimeDomain tdomain;
            tdomain.prepare();
            tdomain->range(new TimeInterval(itDATETIME));
            domain = tdomain; break;
        }
        }
        ColumnDefinition colDef(name, domain,i+1);
        attTable->addColumn(colDef);
    }

}

void GdalTableLoader::loadRecord(Table *attTable, OGRFeatureH hFeature, std::vector<QVariant>& record ) {
    for (int i = 0; i < attTable->columnCount();i++){
        if (_columnFillers[i]){
            record[i] = (this->*_columnFillers[i]->fillFunc)(hFeature, _columnFillers[i]->index);
        }else{
            record[i] = QVariant();
        }
    }
}

void GdalTableLoader::setColumnCallbacks(Table * attTable, OGRLayerH hLayer){
    OGRFeatureDefnH hLayerDef = gdal()->getLayerDef(hLayer);

    _columnFillers.resize(attTable->columnCount(),0);

    for (int i = 0; i < attTable->columnCount(); i++){
        ColumnDefinition& coldef = attTable->columndefinitionRef(i);
        DataDefinition& datadef = coldef.datadef();
        if(datadef.domain().isValid()){
            for (int j = 0; j < gdal()->getFieldCount(hLayerDef); j++){
                OGRFieldDefnH hFieldDefn = gdal()->getFieldDfn(hLayerDef, j);
                QString name = QString(gdal()->getFieldName(hFieldDefn));
                if (coldef.name().compare(name,Qt::CaseSensitive) == 0){
                    IlwisTypes tp = datadef.domain()->valueType();
                    if (tp & itSTRING){
                        _columnFillers[i] = new FillerColumnDef(&GdalTableLoader::fillStringColumn, j);
                    }else if (tp & itINTEGER){
                        NumericRange* r = static_cast<NumericRange*>(datadef.domain()->range<NumericRange>()->clone());
                        //creating the actual range as invalid to be adjusted in the fillers
                        double min = r->min();
                        r->min(r->max());
                        r->max(min);
                        datadef.range(r);
                        _columnFillers[i] = new FillerColumnDef(&GdalTableLoader::fillIntegerColumn, j);
                    }else if (tp & itDOUBLE){
                        //creating the actual range as invalid to be adjusted in the fillers
                        NumericRange* r = static_cast<NumericRange*>(datadef.domain()->range<NumericRange>()->clone());
                        double min = r->min();
                        r->min(r->max());
                        r->max(min);
                        datadef.range(r);
                        _columnFillers[i] = new FillerColumnDef(&GdalTableLoader::fillDoubleColumn, j);
                    }else if (tp == itDATE){
                        _columnFillers[i] = new FillerColumnDef(&GdalTableLoader::fillDateColumn, j);
                    }else if (tp == itTIME){
                        _columnFillers[i] = new FillerColumnDef(&GdalTableLoader::fillTimeColumn, j);
                    }else if (tp == itDATETIME){
                        _columnFillers[i] = new FillerColumnDef(&GdalTableLoader::fillDateTimeColumn, j);
                    }
                }
            }
        }else{
            ERROR2(ERR_NO_INITIALIZED_2, "domain", coldef.name());
        }
    }
}

QVariant GdalTableLoader::fillStringColumn(OGRFeatureH featureH, int colIntex){
    return QVariant(gdal()->getFieldAsString(featureH, colIntex));
}

QVariant GdalTableLoader::fillIntegerColumn(OGRFeatureH featureH, int colIntex){
    int v = gdal()->getFieldAsInt(featureH, colIntex);
    return QVariant(v);
}

QVariant GdalTableLoader::fillDoubleColumn(OGRFeatureH featureH, int colIntex){
    double v = gdal()->getFieldAsDouble(featureH, colIntex);
    return QVariant(v);
}

QVariant GdalTableLoader::fillDateColumn(OGRFeatureH featureH, int colIntex){
    int year,month,day,hour,minute,second,TZFlag;
    if (gdal()->getFieldAsDateTime(featureH,colIntex,&year,&month,&day,&hour,&minute,&second,&TZFlag)){
        return IVARIANT(Time(year, month, day));
    }else
        return QVariant();
}

QVariant GdalTableLoader::fillTimeColumn(OGRFeatureH featureH, int colIntex){
    int year,month,day,hour,minute,second,TZFlag;
    if (gdal()->getFieldAsDateTime(featureH,colIntex,&year,&month,&day,&hour,&minute,&second,&TZFlag)){
        return IVARIANT(Time(hour, minute, (double)second));
    }else
        return QVariant();
}

QVariant GdalTableLoader::fillDateTimeColumn(OGRFeatureH featureH, int colIntex){
    int year,month,day,hour,minute,second,TZFlag;
    if (gdal()->getFieldAsDateTime(featureH,colIntex,&year,&month,&day,&hour,&minute,&second,&TZFlag)){
        return IVARIANT(Time(year, month, day, hour, minute, (double)second));
    }else
        return QVariant();
}

