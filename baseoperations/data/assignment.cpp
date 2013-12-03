#include "kernel.h"
#include "raster.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "polygon.h"
#include "geometry.h"
#include "feature.h"
#include "featurecoverage.h"
#include "featureiterator.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "pixeliterator.h"
#include "mastercatalog.h"
#include "assignment.h"

using namespace Ilwis;
using namespace BaseOperations;

Assignment::Assignment() : _number(rILLEGAL)
{
}


Assignment::Assignment(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr),
    _number(rILLEGAL)
{
}

bool Assignment::assignFeatureCoverage(ExecutionContext *ctx) {

    IFeatureCoverage outputFC = _outputObj.get<FeatureCoverage>();
    IFeatureCoverage inputFC = _inputObj.get<FeatureCoverage>();
    FeatureIterator iterIn(inputFC);
    for_each(iterIn, iterIn.end(), [&](SPFeatureI feature){
        outputFC->newFeatureFrom(feature);
    });

    return true;
}

bool Assignment::assignRasterCoverage(ExecutionContext *ctx) {
    IRasterCoverage outputRaster = _outputObj.get<RasterCoverage>();
    std::function<bool(const Box3D<qint32>)> Assign = [&](const Box3D<qint32> box ) -> bool {
        IRasterCoverage inputRaster = _inputObj.get<RasterCoverage>();
        PixelIterator iterIn(inputRaster, box);
        PixelIterator iterOut(outputRaster, box);

        double v_in = 0;
        //TODO: in principle the stl::copy should work but as yet there is no overload yet(20130621) for
        // + and - operator which is required for this
        //stl::copy(iterIn, iterIn.end(), iterOut);
        for_each(iterOut, iterOut.end(), [&](double& v){
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
        }
    }
    return resource;
}

Ilwis::OperationImplementation *Assignment::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Assignment(metaid, expr);
}

Ilwis::OperationImplementation::State Assignment::prepare(ExecutionContext *, const SymbolTable &)
{
    if ( _expression.parameterCount() != 1) {
        ERROR3(ERR_ILLEGAL_NUM_PARM3,"assignment","1",QString::number(_expression.parameterCount()));
        return sPREPAREFAILED;
    }

    bool ok;
    _number = _expression.parm(0).value().toDouble(&ok);
    if ( !ok) {
        _number = rILLEGAL;
        QString coverage = _expression.parm(0).value();
        Resource resource = mastercatalog()->name2Resource(coverage);
        if ( !resource.isValid()) {
            ERROR1(ERR_COULD_NOT_OPEN_READING_1,coverage);
            return sPREPAREFAILED;
        }
        _inputObj.prepare(coverage, resource.ilwisType());
        OperationHelperRaster helper;
        _outputObj = helper.initialize(_inputObj, resource.ilwisType(),
                                       itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF | itDOMAIN | itTABLE);
        QString outname = _expression.parm(0,false).value();
        if ( outname != sUNDEF)
            _outputObj->setName(outname);
    } else {
        _varName = _expression.parm(0,false).value();
    }

    return sPREPARED;
}

quint64 Assignment::createMetadata()
{
    QString url = QString("ilwis://operations/assignment");
    Resource resource(QUrl(url), itOPERATIONMETADATA);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","assignment");
    resource.addProperty("description",TR("copies the values/properties of the right hand of the expression to the left hand named object"));
    resource.addProperty("syntax","assignment(thing)");
    resource.addProperty("inparameters","1");
    resource.addProperty("pin_1_type", itANY);
    resource.addProperty("pin_1_name", TR("input thing"));
    resource.addProperty("pin_1_desc",TR("input thing"));
    resource.addProperty("pout_1_type", itANY);
    resource.addProperty("pout_1_name", TR("copied thing"));
    resource.addProperty("pout_1_desc",TR(""));
    resource.prepare();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();

}
