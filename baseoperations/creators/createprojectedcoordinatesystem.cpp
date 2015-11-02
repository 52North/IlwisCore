#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "projection.h"
#include "geodeticdatum.h"
#include "coordinatesystem.h"
#include "conventionalcoordinatesystem.h"
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
    return false;
}

Ilwis::OperationImplementation *Ilwis::BaseOperations::CreateProjectedCoordinateSystem::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CreateProjectedCoordinateSystem(metaid,expr);
}

Ilwis::OperationImplementation::State Ilwis::BaseOperations::CreateProjectedCoordinateSystem::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    QString proj;
    QString parameters;
    auto ProjError = [proj](const QString& message)->Ilwis::OperationImplementation::State{
        kernel()->issues()->log(message + proj);
        return sPREPAREFAILED;
    };
    auto NumericParameter = [&](int parmindex, const QString& parm, double& value)->Ilwis::OperationImplementation::State{
        if ( parameters.indexOf(parm) != -1){
            bool ok;
            double v = _expression.input<double>(parmindex,ok);
            if (!ok){
                return ProjError(TR("Invalid projection definition"));
            }
            value = v;

        }
        return sPREPARED;
    };

    proj = _expression.input<QString>(0);
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
        return ProjError(TR("Invalid projection definition"));
    }
    QString code = _projection->code();
    InternalDatabaseConnection db("Select parameters from projection where code='" + code + "'");
    if (!db.next()){
        return ProjError(TR("Invalid projection definition"));
    }
    parameters = db.value(0).toString();
    if (NumericParameter(1,"false easting",_falseeasting) == sPREPAREFAILED)
        return sPREPAREFAILED;
    if (NumericParameter(2,"false northing",_falsenorthing) == sPREPAREFAILED)
        return sPREPAREFAILED;
    if (NumericParameter(3,"central meridian",_centralmeridian) == sPREPAREFAILED)
        return sPREPAREFAILED;
    if (NumericParameter(4,"latitude of origin",_latoforigin) == sPREPAREFAILED)
        return sPREPAREFAILED;
    if (NumericParameter(5,"latitude of true scale",_lattruescale) == sPREPAREFAILED)
        return sPREPAREFAILED;
    if (NumericParameter(6,"standard parallel 1",_standardparallel1) == sPREPAREFAILED)
        return sPREPAREFAILED;
    if (NumericParameter(7,"standard parallel 2",_standardparallel2) == sPREPAREFAILED)
        return sPREPAREFAILED;
    if (NumericParameter(8,"scale factor",_scale) == sPREPAREFAILED)
        return sPREPAREFAILED;
    if (NumericParameter(9,"zone",_zone) == sPREPAREFAILED)
        return sPREPAREFAILED;
    if (NumericParameter(11,"height",_height) == sPREPAREFAILED)
        return sPREPAREFAILED;
    if (NumericParameter(12,"azim central line of true scale",_aztruescale) == sPREPAREFAILED)
        return sPREPAREFAILED;

    _nortoriented = _expression.input<bool>(10);
    if ( _expression.parameterCount() == 15){
        _description = _expression.input<QString>(15);
    }

    return sPREPAREFAILED;
}

quint64 Ilwis::BaseOperations::CreateProjectedCoordinateSystem::createMetadata()
{
    OperationResource resource({"ilwis://operations/createprojectedcoordinatesystem"});
    resource.setLongName("Create Projected Coordinate system");
    resource.setSyntax("createprojectedcoordinatesystem(projectionname,falseeasting,falsenorthing,centralmeridian,latitudeoforigin,standardparallel1,standardparallel2,latitudeoftruescale,scale,zone,height,northoriented,azimtruescale,ellipsoid[,description])");
    resource.setInParameterCount({14,15});
    resource.addInParameter(0, itPROJECTION,TR("Projection name"), TR("Name of the projection used by the system"));
    resource.addInParameter(1, itDOUBLE,TR("False Easting"), TR("A value relating to distance east of a standard meridian but with a constant added to make the numbers convenient"));
    resource.addInParameter(2, itDOUBLE,TR("False Northing"), TR("A value relating to distance north of a standard latitude but with a constant added to make the numbers convenient"));
    resource.addInParameter(3, itDOUBLE,TR("Central Meridian"), TR("a line used to define the origin of the map projection"));
    resource.addInParameter(4, itDOUBLE,TR("Latitude of origin"), TR("defines the origin of the y coordinates. This parameter may not be located at the center of the projection"));
    resource.addInParameter(5, itDOUBLE,TR("Latitude of true scale"), TR("A parallel on a map or chart along which the scale is as stated for that map or chart."));
    resource.addInParameter(6, itDOUBLE,TR("Standard parallel 1"), TR(" used with Conic projections to define the latitude lines where the scale is 1.0"));
    resource.addInParameter(7, itDOUBLE,TR("Standard parallel 2"), TR(" used with Conic projections to define the latitude lines where the scale is 1.0"));
    resource.addInParameter(8, itDOUBLE,TR("Scale"), TR("a unitless value applied to the center point or line of a map projection"));
    resource.addInParameter(9, itINTEGER,TR("Zone"), TR("Zone were the projection gives accurate coordinates"));
    resource.addInParameter(10, itBOOL,TR("North Oriented"), TR("Orientation of the coordinate axis."));
    resource.addInParameter(11, itDOUBLE,TR("Height of perspective"), TR(""));
    resource.addInParameter(12, itDOUBLE,TR("Azimuth of true scale"), TR(""));
    resource.addInParameter(13, itELLIPSOID,TR("Ellipsoid name"), TR("Model of the ellipsoid used to describe the earth"));
    resource.addOptionalInParameter(14, itSTRING, TR("Description"), TR("Description of the newly created coordinatesystem"));
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itCONVENTIONALCOORDSYSTEM, TR("output coordinatesystem"), TR("The newly created projected coordinatesystem"));
    resource.setKeywords("domain, create, identifier, itemdomain");

    mastercatalog()->addItems({resource});
    return resource.id();
}
