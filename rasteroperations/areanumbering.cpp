#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "blockiterator.h"
#include "domainitem.h"
#include "itemrange.h"
#include "identifieritem.h"
#include "itemdomain.h"
#include "areanumbering.h"

using namespace Ilwis;
using namespace RasterOperations;


Ilwis::OperationImplementation *AreaNumbering::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new AreaNumbering(metaid, expr);
}

AreaNumbering::AreaNumbering()
{
}

AreaNumbering::AreaNumbering(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr),
    _connectivity(8)
{
}

bool AreaNumbering::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    IGridCoverage outputGC = _outputObj.get<GridCoverage>();


    BoxedAsyncFunc aggregateFun = [&](const Box3D<qint32>& box) -> bool {
        PixelIterator iterOut(outputGC, box);
        PixelIterator iterIn(_inputObj.get<GridCoverage>());
        AreaNumberer numberer(box.xlength(),_connectivity);
        while(iterOut != iterOut.end()) {
            double v = numberer.value(iterIn) ;
           *iterOut = v;
            ++iterOut;
            ++iterIn;
        }
        return true;
    };
    bool res = OperationHelperRaster::execute(ctx, aggregateFun, outputGC);

    if ( res && ctx != 0) {
        QVariant value;
        value.setValue<IGridCoverage>(outputGC);
        ctx->addOutput(symTable,value,outputGC->name(), itRASTER, outputGC->resource() );
    }
    return res;
}

Ilwis::OperationImplementation::State AreaNumbering::prepare(ExecutionContext *, const SymbolTable & )
{
    QString gc = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();
    int copylist = itCOORDSYSTEM | itGEOREF;

    if (!_inputObj.prepare(gc, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,gc,"");
        return sPREPAREFAILED;
    }
    bool ok;
    _connectivity = _expression.parm(1).value().toUInt(&ok);
    if ( !ok || ( !(_connectivity == 4 || _connectivity == 8))) {
        ERROR2(ERR_ILLEGAL_VALUE_2, "parameter value", "connected number");
        return sPREPAREFAILED;
    }
     _outputObj = OperationHelperRaster::initialize(_inputObj,itRASTER, copylist);
    if ( !_outputObj.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output gridcoverage");
        return sPREPAREFAILED;
    }

    IGridCoverage outputGC = _outputObj.get<GridCoverage>();
    if ( outputName != sUNDEF)
        _outputObj->setName(outputName);

    QString outputBaseName = outputName;
    int index = 0;
    if ( (index = outputName.lastIndexOf(".")) != -1) {
        outputBaseName = outputName.left(index);
    }

    IDomain dom;
    Resource res(outputBaseName, itITEMDOMAIN);
    if (!dom.prepare(res))
        return sPREPAREFAILED;
    outputGC->datadef().domain(dom);


    return sPREPARED;
}

quint64 AreaNumbering::createMetadata()
{
    QString url = QString("ilwis://operations/areanumbering");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","AreaNumbering raster coverage");
    res.addProperty("syntax","areaNumbering(inputgridcoverage,connected (4|8))");
    res.addProperty("description",TR("Area numbering assigns unique pixel values in an output map for connected areas (areas consisting of pixels with the same value, class name, or ID)"));
    res.addProperty("inparameters","2");
    res.addProperty("pin_1_type", itRASTER);
    res.addProperty("pin_1_name", TR("input gridcoverage"));
    res.addProperty("pin_1_desc",TR("input gridcoverage with domain item, boolean or identifier domain"));
    res.addProperty("pin_2_type", itUINT8);
    res.addProperty("pin_2_name", TR("Connectivity"));
    res.addProperty("pin_2_desc",TR("Connected cells, maybe 4 or 8"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itRASTER);
    res.addProperty("pout_1_name", TR("output gridcoverage"));
    res.addProperty("pout_1_desc",TR("output gridcoverage with the identifier domain"));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();
}


//-----------------------------------------------------------
AreaNumberer::AreaNumberer(quint32 xsize, quint8 connectivity) : _connectivity(connectivity)
{
    _neighboursIn.resize(xsize);
    _neighboursOut.resize(xsize);
}

quint32 AreaNumberer::value(const PixelIterator &inIter)
{
    if ( _connectivity == 4) {
        return do4connected(inIter);
    }
    return 0;
}

double AreaNumberer::do4connected(const PixelIterator &in)  {
    qint32 x = in.position().x();
    qint32 y = in.position().x();
    double v = *in;
    double out = rUNDEF;

    if ( x == 0 && y == 0) {
        out = _currentId;
        _neighboursIn[x] = *in;
        _neighboursOut[x] = _currentId++;
    } else if ( y == 0) {
        if ( v == _neighboursIn[x-1] ) {
            out = _neighboursOut[x-1];
            _neighboursIn[x] = v;
            _neighboursOut[x] = _neighboursOut[x-1];
        } else {
            out = _currentId;
            _neighboursIn[x] = v;
            _neighboursOut[x] = _currentId++;
        }
    } else if ( x == 0) {
        if ( v == _neighboursIn[0]) {
            out = _neighboursOut[0];
            _neighboursIn[x] = v;
            _neighboursOut[x] = _neighboursOut[0];
        } else {
            if ( v == _neighboursIn[x-1]) {
                out = _neighboursOut[x-1];
                _neighboursIn[x] = v;
                _neighboursOut[x] = _neighboursOut[x-1];
            }
        }
    } else if ( v == _neighboursIn[x]){
        out = _neighboursOut[x];
        _neighboursIn[x] = v;
        _neighboursOut[x] = _neighboursOut[x];
    } else {
        out = _currentId;
        _neighboursIn[x] = v;
        _neighboursOut[x] = _currentId++;
    }
    return out;
}


