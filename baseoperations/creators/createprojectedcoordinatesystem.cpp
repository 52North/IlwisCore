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
    if ( _envelope.isValid()){
        csy->envelope(_envelope);
    }
    if ( _datumShifts.size() != 0){
        csy->setDatum(new GeodeticDatum(_datumShifts));
    }

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
    auto Optionals = [&](const QString& parameter) -> bool{
        QStringList parts =  parameter.split(",");
        if ( parts.size() == 2) {// its an envelope
            _envelope = Envelope(parameter);
            if (!_envelope.isValid())
                return false;
        }
        if ( parts.size() == 3 || parts.size() == 7 || parts.size() == 10){
            bool ok;
            for(auto part : parts){
                double shift = part.toDouble(&ok);
                if (!ok){
                    kernel()->issues()->log(TR("Invalid values in datum shift"));
                    return false;
                }
                _datumShifts.push_back(shift);
            }
        }
        return true;
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
    if ( _expression.parameterCount() == 4){
        QString p1 = _expression.input<QString>(3).remove('\"');
        if(!Optionals(p1))
            return sPREPAREFAILED;
    }else if(_expression.parameterCount() == 5){
        QString p1 = _expression.input<QString>(3).remove('\"');
        if(!Optionals(p1))
            return sPREPAREFAILED;
        p1 = _expression.input<QString>(4).remove('\"');
        if(!Optionals(p1))
            return sPREPAREFAILED;
    }

    return sPREPARED;
}

quint64 Ilwis::BaseOperations::CreateProjectedCoordinateSystem::createMetadata()
{
    OperationResource resource({"ilwis://operations/createprojectedcoordinatesystem"});
    resource.setLongName("Create Projected Coordinate system");
    resource.setSyntax("createprojectedcoordinatesystem(projectionname,projection parameters,ellipsoid[,datumshifts][,envelope])");
    resource.setInParameterCount({3,4,5});
    resource.addInParameter(0, itSTRING,TR("Projection name"), TR("Name of the projection used by the system"));
    resource.addInParameter(1, itSTRING,TR("Projection Parameters"), TR("A key value pair organized list of the parameters particular to this projection"));
    resource.addInParameter(2, itSTRING,TR("Ellipsoid name"), TR("Name of the ellipsoid used by this coordinate system"));
    resource.addOptionalInParameter(3, itSTRING,TR("Datum shifts"), TR("Optional datum shifts following the toWgs84 convention"));
    resource.addOptionalInParameter(4, itSTRING,TR("Envelope"), TR("Optional envelope limiting the bounds of the coordinate system"));
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itCONVENTIONALCOORDSYSTEM, TR("output coordinatesystem"), TR("The newly created projected coordinatesystem"));
    resource.setKeywords("coordinatesystem, create, projection");

    mastercatalog()->addItems({resource});
    return resource.id();
}
