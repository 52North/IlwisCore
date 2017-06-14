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
#include "serviceobject.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(ServiceObject)

ServiceObject::ServiceObject()
{
}

ServiceObject::ServiceObject(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

void ServiceObject::coordinateSystem(const IIlwisObject& obj, ExecutionContext *ctx, SymbolTable& symTable){
    IConventionalCoordinateSystem csy = obj.as<ConventionalCoordinateSystem>();
    if ( csy.isValid()){
        if (_objectType == "ellipsoid"){
            IEllipsoid ell = csy->ellipsoid();
            setOutput(ell, ctx, symTable);
        }else if ( _objectType == "projection" ){
            IProjection prj = csy->ellipsoid();
            setOutput(prj, ctx, symTable)             ;
        }
    }
}

bool ServiceObject::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    IlwisTypes tp = _object->ilwisType();
    QVariant v;
    if ( hasType(tp, itCOORDSYSTEM)){
        coordinateSystem(_object, ctx, symTable);

    } else if ( hasType(tp, itCOVERAGE)){
        if ( _objectType == "coordinatesystem")
            coordinateSystem(_object.as<Coverage>()->coordinateSystem(), ctx, symTable);
        if ( _objectType == "georeference" && tp == itRASTER){
           setOutput(_object.as<RasterCoverage>()->georeference(), ctx, symTable);
        }
        if ( _objectType == "domain" && tp == itRASTER){
           setOutput(_object.as<RasterCoverage>()->datadef().domain(), ctx, symTable);
        }
    } else if ( hasType(tp, itGEOREF)){
        coordinateSystem(_object.as<GeoReference>()->coordinateSystem(), ctx, symTable);
    }
    return true;
}

Ilwis::OperationImplementation *ServiceObject::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
        return new ServiceObject(metaid, expr);
}

Ilwis::OperationImplementation::State ServiceObject::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString objecturl = _expression.parm(0).value();
    if (!_object.prepare(objecturl)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,objecturl,"");
        return sPREPAREFAILED;
    }
    std::vector<QString> names {"coordinatesystem","georeference","domain","projection","ellipsoid"};
     _objectType = _expression.parm(1).value().toLower().trimmed();
    if (std::find(names.begin(), names.end(), _objectType) == names.end()){
        kernel()->issues()->log(TR("aspect is not in the list of recognized aspecyts:" + _objectType));
        return sPREPAREFAILED;
    }


    return sPREPARED;
}

quint64 ServiceObject::createMetadata()
{

    OperationResource operation({"ilwis://operations/serviceobject"});
    operation.setSyntax("serviceobject(ilwisobject,aspect=!georeference|domain|coordinatesystem|projection|ellipsoid)");
    operation.setDescription(TR("returns value of the indicated aspect as IlwisObject. Not all choices makes sense for all object in which case an undefined value is returned)"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itILWISOBJECT , TR("input object"),TR("Any ilwis object"));
    operation.addInParameter(1,itSTRING, TR("Requested Aspect value"), TR("Any Aspect than can be translted to an ilwis object"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itILWISOBJECT , TR("value"),TR("value of the indicated aspect"));
    operation.setKeywords("tests,workflow");

    mastercatalog()->addItems({operation});
    return operation.id();

}

