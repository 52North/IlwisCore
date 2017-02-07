#include <functional>
#include <future>
#include <algorithm>
#include <cmath>
#include <vector>
#include "kernel.h"
#include "factory.h"
#include "location.h"
#include "abstractfactory.h"
#include "tranquilizerfactory.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "ilwiscontext.h"
#include "relativedem.h"

using namespace Ilwis;
using namespace Hydroflow;

REGISTER_OPERATION(RelativeDEM)

Ilwis::OperationImplementation *RelativeDEM::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RelativeDEM(metaid, expr);
}

RelativeDEM::RelativeDEM()
{
}

RelativeDEM::RelativeDEM(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

Ilwis::OperationImplementation::State RelativeDEM::prepare(ExecutionContext *, const SymbolTable & )
{
    QString dem = _expression.parm(0).value();
//    QString outputName = _expression.parm(0,false).value();

    // open the input DEM
    if (!_inputDEM.prepare(dem, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2, dem, "");
        return sPREPAREFAILED;
    }

    // Open the flow direction map
    QString flowdir = _expression.parm(1).value();
    if (!_inputFlowDir.prepare(flowdir, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2, flowdir, "");
        return sPREPAREFAILED;
    }

    // Open the drainage map
    QString isDrain = _expression.parm(2).value();
    if (!_inputIsDrain.prepare(isDrain, itRASTER)){
        ERROR2(ERR_COULD_NOT_LOAD_2, isDrain, "");
        return sPREPAREFAILED;
    }

    _outputObj = OperationHelperRaster::initialize(_inputDEM, itRASTER, itDOMAIN | itGEOREF | itCOORDSYSTEM | itRASTERSIZE | itBOUNDINGBOX | itENVELOPE);
    if ( !_outputObj.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
        return sPREPAREFAILED;
    }

    IRasterCoverage inputRaster = _inputDEM.as<RasterCoverage>();
    _xsize = inputRaster->size().xsize();
    _ysize = inputRaster->size().ysize();

    // initialize tranquilizer
    initialize(_xsize * _ysize);

    return sPREPARED;
}

bool RelativeDEM::onEdge(Pixel pix) {
    return pix.x == 0 || pix.x == _xsize - 1 ||
           pix.y == 0 || pix.y == _ysize - 1;
}

Pixel RelativeDEM::nextPosition(const Pixel& start, int dir) {
    Pixel next(start);
    switch (dir) {
    case 0 : next.x++;           break;   // E
    case 1 : next.x++; next.y++; break;   // SE
    case 2 :           next.y++; break;   // S
    case 3 : next.x--; next.y++; break;   // SW
    case 4 : next.x--;           break;   // W
    case 5 : next.x--; next.y--; break;   // NW
    case 6 :           next.y--; break;   // N
    case 7 : next.x++; next.y--; break;   // NE
    }

    return next;
}

std::vector<Pixel> RelativeDEM::findStream(const PixelIterator& iterFlow, const PixelIterator& iterIsDrain) {
    std::vector<Pixel> visited;
    PixelIterator flow(iterFlow);
    PixelIterator drain(iterIsDrain);

    Pixel next = flow.position();
    drain(next);
    while ((*flow != rUNDEF) && (*drain == 0)) {  // not reached the drainage yet
        visited.push_back(next);
        next = nextPosition(next, *flow);

        flow[next];
        drain[next];
    }
    visited.push_back(next); // also store the drainage position

    return visited;
}

bool RelativeDEM::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    IRasterCoverage outputRaster = _outputObj.as<RasterCoverage>();
    IRasterCoverage dem = _inputDEM.as<RasterCoverage>();
    IRasterCoverage flowdir = _inputFlowDir.as<RasterCoverage>();
    IRasterCoverage isDrain = _inputIsDrain.as<RasterCoverage>();

    PixelIterator iterDEM(dem, BoundingBox(), PixelIterator::fXYZ);
    PixelIterator iterFlow(flowdir, BoundingBox(), PixelIterator::fXYZ);
    PixelIterator iterIsDrain(isDrain, BoundingBox(), PixelIterator::fXYZ);
    PixelIterator iterOut(outputRaster, BoundingBox(), PixelIterator::fXYZ);
    PixelIterator inEnd = iterFlow.end();

    while (iterFlow != inEnd) {
        trq()->update(1);

        double dir = *iterFlow;
        if ((dir >= 0) && (dir != rUNDEF)) {
            std::vector<Pixel> plist = findStream(iterFlow, iterIsDrain); // plist always has at least one element
            Pixel atLowest = plist[plist.size() - 1];
            double lowest = *(iterDEM[atLowest]);
            double relHeight;
            for (std::vector<Pixel>::iterator it = plist.begin(); it != plist.end(); it++) {
                Pixel pix = *it;
                relHeight = rUNDEF;
                if (!onEdge(pix)) {
                    double outval = *(iterOut[pix]);
                    relHeight = *(iterDEM[pix]) - lowest;
                    if (outval == relHeight)
                        break;  // We can stop now, all next values are already processed earlier
                }

                *(iterOut[pix]) = relHeight;  // relative height wrt (local) drainage
            }
        } else {
            *(iterOut[iterFlow.position()]) = rUNDEF;
        }
        iterFlow += 1;
    }

    trq()->inform("\nWriting...\n");
    trq()->stop();

    bool resource = true;
    if ( resource && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(outputRaster);
        ctx->setOutput(symTable, value, outputRaster->name(), itRASTER, outputRaster->resource() );
    }
    return resource;
}

quint64 RelativeDEM::createMetadata()
{
    OperationResource operation({"ilwis://operations/relativedem"});
    operation.setLongName("Relative height above drainage");
    operation.setSyntax("relativedem(inputdemcoverage,inputflowdircoverage,inputisdrain)");
    operation.setDescription(TR("Determine the relative height towards the closest drainage following the flow direction"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itRASTER, TR("DEM"),TR("Input map with absolute heights"));
    operation.addInParameter(1,itRASTER, TR("Flow direction"),TR("Input map with the flow direction in each location"));
    operation.addInParameter(2,itRASTER, TR("Drainage map"),TR("Input map indicating if a location belongs to the drainage network") );
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output rastercoverage"), TR("Relative height"));
    operation.setKeywords("raster, DEM, flow direction, drainage, relative height");

    mastercatalog()->addItems({operation});
    return operation.id();
}
