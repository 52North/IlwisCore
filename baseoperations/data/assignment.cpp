#include "raster.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "pixeliterator.h"
#include "mastercatalog.h"
#include "assignment.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(Assignment)

Assignment::Assignment() : _number(rILLEGAL)
{
}


Assignment::Assignment(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr),
    _number(rILLEGAL)
{
}

bool Assignment::assignFeatureCoverage(ExecutionContext *ctx) {

    IFeatureCoverage outputFC = _outputObj.as<FeatureCoverage>();
    IFeatureCoverage inputFC = _inputObj.as<FeatureCoverage>();
    FeatureIterator iterIn(inputFC);
    for_each(iterIn, iterIn.end(), [&](SPFeatureI feature){
        outputFC->newFeatureFrom(feature);
    });

    return true;
}

bool Assignment::assignTable(ExecutionContext *ctx) {

    ITable outputFC = _outputObj.as<Table>();
    ITable inputFC = _inputObj.as<Table>();
    outputFC = inputFC->copyTable(ANONYMOUS_PREFIX);

    return true;
}

bool Assignment::assignRasterCoverage(ExecutionContext *ctx) {
    IRasterCoverage outputRaster = _outputObj.as<RasterCoverage>();
    std::function<bool(const BoundingBox)> Assign = [&](const BoundingBox box ) -> bool {
        IRasterCoverage inputRaster = _inputObj.as<RasterCoverage>();
        PixelIterator iterIn(inputRaster, box);
        PixelIterator iterOut(outputRaster, box);

        double v_in = 0;
        //TODO: in principle the stl::copy should work but as yet there is no overload yet(20130621) for
        // + and - operator which is required for this
        //stl::copy(iterIn, iterIn.end(), iterOut);
        std::for_each(iterOut, iterOut.end(), [&](double& v){
            v_in = *iterIn;
            if ( v_in != rUNDEF) {
                v = v_in;
            }
            ++iterIn;
            ++iterOut;
        });
        return true;
    };

    return  OperationHelperRaster::execute(ctx, Assign, outputRaster);

}

bool Assignment::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    bool resource = false;
    if ( _number != rILLEGAL) {

    } else {
        if ( _inputObj.isValid()) {
            if ( _inputObj->ilwisType() == itRASTER) {
                if((resource = assignRasterCoverage(ctx)) == true)
                    setOutput<RasterCoverage>(ctx, symTable);
            }
            if ( (_inputObj->ilwisType() & itFEATURE)!= 0) {
                if((resource = assignFeatureCoverage(ctx)) == true)
                    setOutput<FeatureCoverage>(ctx, symTable);
            }
            if ( (_inputObj->ilwisType() & itTABLE)!= 0) {
                if((resource = assignTable(ctx)) == true)
                    setOutput<Table>(ctx, symTable);
            }
        }
    }
    return resource;
}

Ilwis::OperationImplementation *Assignment::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Assignment(metaid, expr);
}

Ilwis::OperationImplementation::State Assignment::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    if ( _expression.parameterCount() != 1) {
        ERROR3(ERR_ILLEGAL_NUM_PARM3,"assignment","1",QString::number(_expression.parameterCount()));
        return sPREPAREFAILED;
    }

    bool ok;
    _number = _expression.parm(0).value().toDouble(&ok);
    if ( !ok) {
        _number = rILLEGAL;
        QString coverage = _expression.parm(0).value();
        quint64 id = mastercatalog()->name2id(coverage);
//        if ( !resource.isValid()) {
//            ERROR1(ERR_COULD_NOT_OPEN_READING_1,coverage);
//            return sPREPAREFAILED;
//        }
//        _inputObj.prepare(coverage, resource.ilwisType());

        _inputObj.prepare(id);
        if(!_inputObj->isValid()){
            ERROR1(ERR_COULD_NOT_OPEN_READING_1,coverage);
            return sPREPAREFAILED;
        }
        OperationHelperRaster helper;
        _outputObj = helper.initialize(_inputObj, _inputObj->ilwisType(),
                                       itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF | itDOMAIN | itTABLE);
        QString outname = _expression.parm(0,false).value();
        if ( outname != sUNDEF)
            _outputObj->name(outname);
    } else {
        _varName = _expression.parm(0,false).value();
    }

    return sPREPARED;
}

quint64 Assignment::createMetadata()
{
    OperationResource operation({"ilwis://operations/assignment"});
    operation.setSyntax("assignment(thing)");
    operation.setDescription(TR("copies the values/properties of the right hand of the expression to the left hand named object"));
    operation.setInParameterCount({1});
    operation.addInParameter(0,itRASTER | itFEATURE , TR("input thing"),TR("input thing"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER | itFEATURE, TR("copy of the object"));
    operation.setKeywords("internal");

    mastercatalog()->addItems({operation});
    return operation.id();

}
