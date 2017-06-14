#include "kernel.h"
#include "raster.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "coordinatesystem.h"
#include "ellipsoid.h"
#include "projection.h"
#include "proj4parameters.h"
#include "conventionalcoordinatesystem.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "property.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(Property)

Property::Property()
{
}

Property::Property(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

bool Property::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    IlwisTypes tp = _object->ilwisType();
    QVariant v;
    IlwisTypes valueType = itUNKNOWN;
    if ( _property == "name")
        v = _object->name();
    else if ( _property == "type"){
        v = TypeHelper::type2name(_object->ilwisType());
    }else if ( _property == "url"){
        v = _object->resource().url().toString();
    }else if ( _property == "rawurl"){
        v = _object->resource().url(true).toString();
    }
    if ( hasType(tp, itRASTER) ){
        IRasterCoverage raster = _object.as<RasterCoverage>();
        if ( _property == "valuetype"){
            v =  TypeHelper::type2name(raster->datadef().domain()->valueType());
        }else if (raster->datadef().domain()->ilwisType() == itNUMERICDOMAIN &&  _property == "max" ){
            v = raster->datadef().range()->as<NumericRange>()->max();
        }else if (raster->datadef().domain()->ilwisType() == itNUMERICDOMAIN && _property == "min" ){
            v = raster->datadef().range()->as<NumericRange>()->min();
        }
    }
    if ( hasType(tp, itCOVERAGE|itCOORDSYSTEM|itTABLE|itGEOREF)){
        if ( _property == "xsize" || _property == "ysize" || _property == "zsize" || _property == "rowcount"
             || _property == "columncount" ){
            if ( hasType(tp, itRASTER)){
                IRasterCoverage raster = _object.as<RasterCoverage>();
                if ( _property == "rowcount" || _property == "columncount"){
                    v = _property == "rowcount" ? raster->size().xsize() : raster->size().ysize();
                }else
                    v = _property == "xsize" ? raster->size().xsize() : (_property == "ysize" ? raster->size().ysize() : raster->size().zsize());
                valueType = itINTEGER;
            }else if ( hasType(tp, itCOORDSYSTEM)){
                ICoordinateSystem csy = _object.as<CoordinateSystem>();
                v = _property == "xsize" ? csy->envelope().size().xsize() : csy->envelope().size().ysize();
                valueType = itDOUBLE;
            }else if ( hasType(tp, itGEOREF)){
                IGeoReference grf = _object.as<GeoReference>();
                if ( _property == "rowcount" || _property == "columncount"){
                    v = _property == "rowcount" ? grf->size().xsize() : grf->size().ysize();
                }else
                    v = _property == "xsize" ? grf->size().xsize() : grf->size().ysize();
                valueType = itINTEGER;
            }else if ( hasType(tp, itTABLE)){
                ITable tbl = _object.as<Table>();
                if ( _property == "rowcount" || _property == "columcount"){
                    v = _property == "rowcount" ?  tbl->columnCount() : tbl->recordCount();
                }else
                    v = _property == "xsize" ? tbl->columnCount() : tbl->recordCount();
                valueType = itINTEGER;
            }
        }
    }else if ( hasType(tp, itFEATURE)){
        IFeatureCoverage features = _object.as<FeatureCoverage>();
        if ( _property == "featurecount"){
            v = features->featureCount();
        }else if ( _property == "pointcount"){
            v = features->featureCount(itPOINT);
        }else if ( _property == "linecount"){
            v = features->featureCount(itLINE);
        }else if ( _property == "polygoncount"){
            v = features->featureCount(itPOLYGON);
        }
    }else if ( hasType(tp, itCOORDSYSTEM)){
        IConventionalCoordinateSystem csy = _object.as<ConventionalCoordinateSystem>();
        if ( csy.isValid()){
            if ( _property == "ellipsoid"){
                v =  csy->ellipsoid()->code(); // or name??
            }else if ( _property == "projection"){
                v =  csy->projection()->name();
            }
        }
    }else if ( hasType(tp, itCOORDSYSTEM)){
        IDomain dom = _object.as<Domain>();
        if ( _property == "valuetype"){
            v =  TypeHelper::type2name(dom->valueType());
        }
    }
    if ( !v.isValid()){
        v = sUNDEF;
        valueType = itSTRING;
    }
    logOperation(_expression);
    ctx->setOutput(symTable, v, sUNDEF, valueType, Resource());
    return true;
}

Ilwis::OperationImplementation *Property::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
        return new Property(metaid, expr);
}

Ilwis::OperationImplementation::State Property::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString objecturl = _expression.parm(0).value();
    if (!_object.prepare(objecturl)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,objecturl,"");
        return sPREPAREFAILED;
    }
    std::vector<QString> names {"name","xsize","ysize","zsize","rowcount","columncount","type","valuetype","featurecount","pointcount","linecount","polygoncount","pixelcount","domain","projection","ellipsoid","url","rawurl","min","max"};
     _property = _expression.parm(1).value().toLower().trimmed();
    if (std::find(names.begin(), names.end(), _property) == names.end()){
        kernel()->issues()->log(TR("Property is not in the list of recognized properties:" + _property));
        return sPREPAREFAILED;
    }


    return sPREPARED;
}

quint64 Property::createMetadata()
{

    OperationResource operation({"ilwis://operations/property"});
    operation.setSyntax("property(ilwisobject,property=!name|xsize|ysize|zsize|rows|columns|type|valuetype|featurecount|pointcount|linecount|polygoncount|pixelcount|domain|projection|ellipsoid|url|rawurl|min|max)");
    operation.setDescription(TR("returns value of the indicated property. Not all choices makes sense for all object in which case an undefined value is returned)"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itILWISOBJECT , TR("input object"),TR("Any ilwis object"));
    operation.addInParameter(1,itSTRING, TR("Requested property value"), TR("Any property than can be translted to a string or number"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itDOUBLE|itSTRING , TR("value"),TR("value of the indicated property"));
    operation.setKeywords("tests,workflow");

    mastercatalog()->addItems({operation});
    return operation.id();

}

