#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "projection.h"
#include "geodeticdatum.h"
#include "coordinatesystem.h"
#include "conventionalcoordinatesystem.h"
#include "ellipsoid.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "createprojectedcoordinatesystem.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(CreateProjectedCoordinateSystem)

CreateProjectedCoordinateSystem::CreateProjectedCoordinateSystem()
{

}

Ilwis::BaseOperations::CreateProjectedCoordinateSystem::CreateProjectedCoordinateSystem(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool Ilwis::BaseOperations::CreateProjectedCoordinateSystem::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    IConventionalCoordinateSystem csy;
    csy.prepare();
    for(auto kvp : _parms) {
        _projection->setParameter( Projection::parameterName2type(kvp.first), kvp.second);
    }
    csy->setProjection(_projection);
    csy->setEllipsoid(_ellipsoid);

    QVariant value;
    value.setValue<ICoordinateSystem>(csy);
    ctx->setOutput(symTable,value,csy->name(),itCOORDSYSTEM,csy->source());
    return true;
}

Ilwis::OperationImplementation *Ilwis::BaseOperations::CreateProjectedCoordinateSystem::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CreateProjectedCoordinateSystem(metaid,expr);
}

Ilwis::OperationImplementation::State Ilwis::BaseOperations::CreateProjectedCoordinateSystem::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    QString proj;
    QString parameters = _expression.input<QString>(1).remove('\"');
    auto ProjError = [proj](const QString& message)->bool{
        kernel()->issues()->log(message + proj);
        return false;
    };
    auto NumericParameter = [&](const QString& parm)->bool{
        if ( parameters.indexOf(parm) != -1){
            bool ok = false;
            QStringList parts = parameters.split(",");
            for(auto part : parts){
                if ( part.indexOf(parm) == 0){
                    QString value = part.split("=").last();
                    value = value.trimmed();
                    _parms[parm] = value.toDouble(&ok);
                    return true;
                }
            }
            if (!ok){
                return ProjError(TR("Invalid projection definition"));
            }
        }
        return false;
    };

    proj = _expression.input<QString>(0);
    proj = proj.remove('\"');
    if ( proj.indexOf("ilwis:/") != -1){
        _projection.prepare(proj);
    }else {
        InternalDatabaseConnection db("Select code from projection where name='" + proj + "'");
        if ( db.next()){
            QString code = db.value(0).toString();
            _projection.prepare("ilwis://tables/projection?code=" + code);
        }
    }
    if ( !_projection.isValid()){
        ProjError(TR("Invalid projection definition"));
        return sPREPAREFAILED;
    }
    QStringList parameterNameList = _projection->parameterNameList();

    for(int index = 0; index < parameterNameList.size(); ++index) {
        bool ok = NumericParameter(parameterNameList[index]);
        if (!ok)
            return sPREPAREFAILED;
    }
    QString ell = _expression.input<QString>(2);
    ell = ell.remove('\"');

    if ( ell.indexOf("ilwis:/") != -1){
        _ellipsoid.prepare(ell);
    }else {
        InternalDatabaseConnection db("Select code from ellipsoid where name='" + ell + "'");
        if ( db.next()){
            QString code = db.value(0).toString();
            _ellipsoid.prepare("ilwis://tables/ellipsoid?code=" + code);
        }
    }

    return sPREPARED;
}

quint64 Ilwis::BaseOperations::CreateProjectedCoordinateSystem::createMetadata()
{
    OperationResource resource({"ilwis://operations/createprojectedcoordinatesystem"});
    resource.setLongName("Create Projected Coordinate system");
    resource.setSyntax("createprojectedcoordinatesystem(projectionname,falseeasting,falsenorthing,centralmeridian,latitudeoforigin,standardparallel1,standardparallel2,latitudeoftruescale,scale,zone,height,northoriented,azimtruescale,ellipsoid[,description])");
    resource.setInParameterCount({3});
    resource.addInParameter(0, itSTRING,TR("Projection name"), TR("Name of the projection used by the system"));
    resource.addInParameter(1, itSTRING,TR("Projection Parameters"), TR("A key value pair organized list of the parameters particular to this projection"));
    resource.addInParameter(2, itSTRING,TR("Ellipsoid name"), TR("Name of the ellipsoid used by this coordinate system"));
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itCONVENTIONALCOORDSYSTEM, TR("output coordinatesystem"), TR("The newly created projected coordinatesystem"));
    resource.setKeywords("coordinatesystem, create, projection");

    mastercatalog()->addItems({resource});
    return resource.id();
}
