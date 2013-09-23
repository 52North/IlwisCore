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
#include "identifierrange.h"
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

    IRasterCoverage outputGC = _outputObj.get<RasterCoverage>();
    AreaNumberer numberer(outputGC->size().xsize(),_connectivity);

    BoxedAsyncFunc aggregateFun = [&](const Box3D<qint32>& box) -> bool {
        //pass one
        PixelIterator iterOut(outputGC, box);
        PixelIterator iterIn(_inputObj.get<RasterCoverage>());
        PixelIterator iterEnd = iterOut.end();
        while(iterOut != iterEnd) {
            double v = numberer.value(iterIn) ;
           *iterOut = v;
            ++iterOut;
            ++iterIn;
        }
        //pass two
        for(auto iterPass2 : numberer.pass2Entries()) {
            for(double& v : _outputObj.get<RasterCoverage>()){
                if ( v == iterPass2.first){
                    v = iterPass2.second;
                }
            }
        }

        return true;
    };
    ctx->_threaded = false; // operation can not be run in parallel
    bool res = OperationHelperRaster::execute(ctx, aggregateFun, outputGC);

    INamedIdDomain iddom = outputGC->datadef().domain().get<NamedIdDomain>();
    NamedIdentifierRange range;
    for(int i=0; i < numberer.lastid(); ++i) {
        range << QString("area_%1").arg(i);
    }
    iddom->setRange(range);

    if ( res && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(outputGC);
        ctx->addOutput(symTable,value,outputGC->name(), itRASTER, outputGC->resource() );
    }
    return res;
}

Ilwis::OperationImplementation::State AreaNumbering::prepare(ExecutionContext *, const SymbolTable & )
{
    QString rasterCoverage = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();
    int copylist = itCOORDSYSTEM | itGEOREF;

    if (!_inputObj.prepare(rasterCoverage, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,rasterCoverage,"");
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

    IRasterCoverage outputGC = _outputObj.get<RasterCoverage>();
    if ( outputName != sUNDEF)
        _outputObj->setName(outputName);

    QString outputBaseName = outputName;
    int index = 0;
    if ( (index = outputName.lastIndexOf(".")) != -1) {
        outputBaseName = outputName.left(index);
    }

    IDomain dom;
    QString domname = QString("ilwis://internalcatalog/%1").arg(outputBaseName);
    Resource res(QUrl(domname), itITEMDOMAIN | itNAMEDITEM);
    if (!dom.prepare(res))
        return sPREPAREFAILED;
    outputGC->datadef().domain(dom);
    mastercatalog()->addItems({res});


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

std::map<int, int> AreaNumberer::pass2Entries()
{
    return _pass2Changes;
}

quint32 AreaNumberer::value(const PixelIterator &inIter)
{
    if ( _connectivity == 4) {
        return do4connected(inIter);
    }
    return 0;
}

quint32 AreaNumberer::lastid() const
{
    return _currentId;
}

double AreaNumberer::do4connected(const PixelIterator &in)  {
    qint32 x = in.position().x();
    qint32 y = in.position().y();
    double v = *in;
    double out = rUNDEF;

    if ( x == 0 && y == 0) {
        out = _currentId;
        _neighboursIn[x] = v;
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
    } else if ( v == _neighboursIn[x] ){
        out = _neighboursOut[x];
        _neighboursIn[x] = v;
        if ( x > 0 && y > 0 &&
             v == _neighboursIn[x - 1] &&
             _neighboursOut[x] != _neighboursOut[x-1]){
            _pass2Changes[_neighboursOut[x-1]] = _neighboursOut[x];
        }
    } else if ( x > 0 && v == _neighboursIn[x - 1]) {
        out = _neighboursOut[x - 1];
        _neighboursIn[x] = v;
        _neighboursOut[x] = _neighboursOut[x - 1];
    } else {
        out = _currentId;
        _neighboursIn[x] = v;
        _neighboursOut[x] = _currentId++;
    }
    return out;
}


