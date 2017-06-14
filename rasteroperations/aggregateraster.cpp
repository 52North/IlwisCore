#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "blockiterator.h"
#include "aggregateraster.h"

using namespace Ilwis;
using namespace RasterOperations;


Ilwis::OperationImplementation *AggregateRaster::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new AggregateRaster(metaid, expr);
}

AggregateRaster::AggregateRaster()
{
}

AggregateRaster::AggregateRaster(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr),
    _method(NumericStatistics::pSUM)
{
}


void AggregateRaster::executeGrouped(const BoundingBox& inpBox){
    BlockIterator blockInputIter(_inputObj.as<RasterCoverage>(),Size<>(groupSize(0),groupSize(1), groupSize(2)), inpBox);
    PixelIterator iterOut(_outputObj.as<RasterCoverage>());
    PixelIterator iterEnd = iterOut.end();
    quint64 currentCount = 0;
    while(iterOut != iterEnd) {
        GridBlock& block = *blockInputIter;
        std::vector<double> values= (*blockInputIter).toVector();
        double v = OperationHelper::statisticalMarker(values, _method);
        *iterOut = v;
        ++iterOut;
        ++blockInputIter;

        //updateTranquilizer(currentCount++, 1000);
    }
}

void AggregateRaster::executeNonGrouped(const BoundingBox& inpBox){
    BlockIterator blockInputIter(_inputObj.as<RasterCoverage>(),Size<>(groupSize(0),groupSize(1), groupSize(2)), inpBox);
    IRasterCoverage outRaster = _outputObj.as<RasterCoverage>();
    BlockIterator blockOutputIter(outRaster,Size<>(groupSize(0),groupSize(1), groupSize(2)), inpBox);
    BlockIterator iterEnd = blockOutputIter.end();
    quint64 currentCount = 0;
    while(blockOutputIter != iterEnd) {
        std::vector<double> values= (*blockInputIter).toVector();
        double v = OperationHelper::statisticalMarker(values, _method);
        GridBlock& blockOut = *blockOutputIter;
        std::fill(blockOut.begin(), blockOut.end(), v);
        ++blockInputIter;
        ++blockOutputIter;

        //updateTranquilizer(currentCount++, 1000);
    }
}

bool AggregateRaster::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    IRasterCoverage outputRaster = _outputObj.as<RasterCoverage>();


    BoxedAsyncFunc aggregateFun = [&](const BoundingBox& box) -> bool {
        //Size sz = outputRaster->size();
        PixelIterator iterOut(outputRaster, box);
        BoundingBox inpBox(Pixel(box.min_corner().x,
                                             box.min_corner().y * groupSize(1),
                                             box.min_corner().z * groupSize(2)),
                             Pixel((box.max_corner().x+1) * groupSize(0) - 1,
                                             (box.max_corner().y + 1) * groupSize(1) - 1,
                                             (box.max_corner().z + 1) * groupSize(2) - 1) );

            if ( _grouped)
                executeGrouped( inpBox);
            else
                executeNonGrouped( inpBox);
        return true;
    };
    bool res = OperationHelperRaster::execute(ctx, aggregateFun, outputRaster);

    if ( res && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(outputRaster);
        logOperation(outputRaster,_expression);
        ctx->setOutput(symTable,value,outputRaster->name(), itRASTER, outputRaster->resource() );
    }
    return res;
}

NumericStatistics::PropertySets AggregateRaster::toMethod(const QString& nm) {
    QString mname = nm.toLower();
    if ( mname == "avg")
        return NumericStatistics::pMEAN;
    else if ( mname == "min")
        return NumericStatistics::pMIN;
    else if ( mname == "max")
        return NumericStatistics::pMAX;
    else if ( mname == "med")
        return NumericStatistics::pMEDIAN;
    else if ( mname == "pred")
        return NumericStatistics::pPREDOMINANT;
    else if ( mname == "sum")
        return NumericStatistics::pSUM;

    return NumericStatistics::pLAST    ;
}

Ilwis::OperationImplementation::State AggregateRaster::prepare(ExecutionContext *ctx, const SymbolTable &st )
{
    OperationImplementation::prepare(ctx,st);
    QString raster = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();
    int copylist = itDOMAIN | itCOORDSYSTEM;

    if (!_inputObj.prepare(raster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
        return sPREPAREFAILED;
    }
    _method = toMethod(_expression.parm(1).value());
    if ( _method == NumericStatistics::pLAST) {
        ERROR2(ERR_ILLEGAL_VALUE_2, "parameter value", " aggregation method");
        return sPREPAREFAILED;
    }
    bool ok;
    quint32 groupSz = _expression.parm(2).value().toInt(&ok);
    if (!ok) {
        QString blist = _expression.parm(2).value();
        blist.remove("{");
        blist.remove("}");
        QStringList dims = blist.split(" ");
        if ( dims.size() > 0) {
            for(int i=0; i < dims.size(); ++i)     {
                quint32 val = dims[i].toInt(&ok);
                if ( ok) {
                    _groupSize[i] = val;
                }else
                    break;
            }
        }

    }else {
        _groupSize[0] = groupSz;
        _groupSize[1] = groupSz;

    }
    if ( !ok || groupSize() < 2) {
        ERROR2(ERR_ILLEGAL_VALUE_2, "aggregation group size", QString::number(groupSize()));
        return sPREPAREFAILED;
    }

    QString grouped =  _expression.parm(3).value().toLower();
    _grouped = grouped == "true" || grouped == "yes" || grouped == "1";
    if ( !_grouped)
        copylist |= itGEOREF;

    _outputObj = OperationHelperRaster::initialize(_inputObj,itRASTER, copylist);
    if ( !_outputObj.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
        return sPREPAREFAILED;
    }
    QString outputBaseName = outputName;
    int index = 0;
    if ( (index = outputName.lastIndexOf(".")) != -1) {
        outputBaseName = outputName.left(index);
    }
    IRasterCoverage inputRaster = _inputObj.as<RasterCoverage>();
    IRasterCoverage outputRaster = _outputObj.as<RasterCoverage>();
    if ( outputName != sUNDEF)
        _outputObj->name(outputName);
    outputRaster->coordinateSystem(inputRaster->coordinateSystem());

    BoundingBox box(inputRaster->size());
    if ( _grouped) {
        int xs = box.xlength();
        int ys = box.ylength();
        int zs = box.zlength();
        int newxs = xs / groupSize(0);
        int newys = ys / groupSize(1);
        int newzs = zs / groupSize(2);
        box = BoundingBox(Size<>(newxs, newys, newzs));

    }
    if ( _grouped) {
        Envelope envlope = inputRaster->envelope();
        Resource resource(QUrl("ilwis://internalcatalog/georeference" + _outputObj->name()),itGEOREF);
        resource.addProperty("size", IVARIANT(box.size()));
        resource.addProperty("envelope", IVARIANT(envlope));
        resource.addProperty("coordinatesystem", inputRaster->coordinateSystem()->id());
        resource.addProperty("name", outputBaseName);
        resource.addProperty("centerofpixel",inputRaster->georeference()->centerOfPixel());
        IGeoReference  grf;
        if (grf.prepare(resource)) {
            mastercatalog()->addItems({resource});
            outputRaster->georeference(grf);
        }
        outputRaster->envelope(envlope);
        outputRaster->size(box.size());
    }

    initialize(outputRaster->size().linearSize());

    return sPREPARED;
}

quint64 AggregateRaster::createMetadata()
{

    OperationResource operation({"ilwis://operations/aggregateraster"});
    operation.setLongName("Spatial Raster Aggregation");
    operation.setSyntax("aggregateraster(inputgridcoverage,aggregationmethod=!Avg|Max|Med|Min|Prd|Sum, groupsize,changegeometry)");
    operation.setDescription(TR("generates a rastercoverage according to a aggregation method. The aggregation method determines how pixel values are used in the aggregation"));
    operation.setInParameterCount({4});
    operation.addInParameter(0,itRASTER , TR("input rastercoverage"),TR("input rastercoverage with any domain"));
    operation.addInParameter(1,itSTRING , TR("Aggregation Method"),TR("the method how pixels inside a group will be accumulated"));
    operation.addInParameter(2,itINTEGER | itSTRING , TR("Groupsize"),TR("The size of the block used to aggregate. In the case of integer it is a square 2D block; in the case of string it is of the list format (2 or 3 dimensions). eg {3 4}"));
    operation.addInParameter(3,itBOOL , TR("change geometry"),TR("The aggregation can either create a map with a reduced size proportional to de block size or use the same geometry size but fill all pixels in the block with the aggregate"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("Aggregated raster"), TR("output rastercoverage with the domain of the input map"));
    operation.setKeywords("aggregate,raster,geometry");

    mastercatalog()->addItems({operation});
    return operation.id();
}

quint32 AggregateRaster::groupSize(int dim)
{
    return _groupSize[dim];
}


