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

bool AggregateRaster::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    IRasterCoverage outputGC = _outputObj.get<RasterCoverage>();


    BoxedAsyncFunc aggregateFun = [&](const Box3D<qint32>& box) -> bool {
        //Size sz = outputGC->size();
        PixelIterator iterOut(outputGC, box);
        Box3D<qint32> inpBox(Point3D<qint32>(box.min_corner().x(),
                                             box.min_corner().y() * groupSize(1),
                                             box.min_corner().z() * groupSize(2)),
                             Point3D<qint32>((box.max_corner().x()+1) * groupSize(0) - 1,
                                             (box.max_corner().y() + 1) * groupSize(1) - 1,
                                             (box.max_corner().z() + 1) * groupSize(2) - 1) );

        BlockIterator blockIter(_inputObj.get<RasterCoverage>(),Size(groupSize(0),groupSize(1), groupSize(2)), inpBox);
        NumericStatistics stats;
        PixelIterator iterEnd = iterOut.end();
        while(iterOut != iterEnd) {
            GridBlock& block = *blockIter;
            stats.calculate(block.begin(), block.end(), _method);
            double v = stats[_method];
           *iterOut = v;
            ++iterOut;
            ++blockIter;
        }
        return true;
    };
    bool res = OperationHelperRaster::execute(ctx, aggregateFun, outputGC);

    if ( res && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(outputGC);
        ctx->addOutput(symTable,value,outputGC->name(), itRASTER, outputGC->resource() );
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
    else if ( mname == "std")
        return NumericStatistics::pSTDEV;
    else if ( mname == "sum")
        return NumericStatistics::pSUM;

    return NumericStatistics::pLAST    ;
}

Ilwis::OperationImplementation::State AggregateRaster::prepare(ExecutionContext *, const SymbolTable & )
{
    QString gc = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();
    int copylist = itDOMAIN | itCOORDSYSTEM;

    if (!_inputObj.prepare(gc, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,gc,"");
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

    _grouped = _expression.parm(3).value() == "true";
    if ( !_grouped)
        copylist |= itGEOREF;

    _outputObj = OperationHelperRaster::initialize(_inputObj,itRASTER, copylist);
    if ( !_outputObj.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output gridcoverage");
        return sPREPAREFAILED;
    }
    QString outputBaseName = outputName;
    int index = 0;
    if ( (index = outputName.lastIndexOf(".")) != -1) {
        outputBaseName = outputName.left(index);
    }
    IRasterCoverage inputGC = _inputObj.get<RasterCoverage>();
    IRasterCoverage outputGC = _outputObj.get<RasterCoverage>();
    if ( outputName != sUNDEF)
        _outputObj->setName(outputName);

    Box3D<qint32> box(inputGC->size());
    if ( _grouped) {
        int xs = box.xlength();
        int ys = box.ylength();
        int zs = box.zlength();
        int newxs = xs / groupSize(0);
        int newys = ys / groupSize(1);
        int newzs = zs / groupSize(2);
        box = Box3D<qint32>(Size(newxs, newys, newzs));

    }
    if ( _expression.parameterCount() == 5 || _grouped) {
        Box2D<double> envlope = inputGC->envelope();
        Resource res(QUrl("ilwis://internal/georeference"),itGEOREF);
        res.addProperty("size", IVARIANT(box.size()));
        res.addProperty("envelope", IVARIANT(envlope));
        res.addProperty("coordinatesystem", IVARIANT(inputGC->coordinateSystem()));
        res.addProperty("name", outputBaseName);
        res.addProperty("centerofpixel",inputGC->georeference()->centerOfPixel());
        IGeoReference  grf;
        grf.prepare(res);
        outputGC->georeference(grf);
        outputGC->envelope(envlope);
        outputGC->size(box.size());
    }

    return sPREPARED;
}

quint64 AggregateRaster::createMetadata()
{
    QString url = QString("ilwis://operations/aggregateraster");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","aggregateraster raster coverage");
    res.addProperty("syntax","aggregateraster(inputgridcoverage,{Avg|Max|Med|Min|Prd|Std|Sum}, groupsize,changegeometry[,new georefname])");
    res.addProperty("description",TR("generates a gridcoverage according to a aggregation method. The aggregation method determines how pixel values are used in the aggregation "));
    res.addProperty("inparameters","4|5");
    res.addProperty("pin_1_type", itRASTER);
    res.addProperty("pin_1_name", TR("input gridcoverage"));
    res.addProperty("pin_1_desc",TR("input gridcoverage with domain any domain"));
    res.addProperty("pin_2_type", itSTRING);
    res.addProperty("pin_2_name", TR("Aggregation Method"));
    res.addProperty("pin_2_desc",TR("the method how pixels inside a group will be accumulated"));
    res.addProperty("pin_3_type", itINTEGER | itSTRING);
    res.addProperty("pin_3_name", TR("Groupsize"));
    res.addProperty("pin_3_desc",TR("The size of the block used to aggregate. In the case of integer it is a square 2D block; in the case of string it is of the list format (2 or 3 dimensions). eg {3 4}"));
    res.addProperty("pin_4_type", itBOOL);
    res.addProperty("pin_4_name", TR("change geometry"));
    res.addProperty("pin_4_desc",TR("The aggregation can either create a map with a reduced size proportional to de block size or use the same geometry size but fill all pixels in the block with the aggregate"));
    res.addProperty("pin_5_type", itSTRING);
    res.addProperty("pin_5_name", TR("georeference name"));
    res.addProperty("pin_5_desc",TR("optional parameter indicating a name for the new geometry, else the name will come from the output grid"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itRASTER);
    res.addProperty("pout_1_name", TR("output gridcoverage"));
    res.addProperty("pout_1_desc",TR("output gridcoverage with the domain of the input map"));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();
}

quint32 AggregateRaster::groupSize(int dim)
{
    return _groupSize[dim];
}


