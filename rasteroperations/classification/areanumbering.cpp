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

REGISTER_OPERATION(AreaNumbering)

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

    IRasterCoverage outputRaster = _outputObj.as<RasterCoverage>();
    AreaNumberer numberer(outputRaster->size().xsize(),_connectivity);
    quint64 currentCount = 0;

    BoxedAsyncFunc aggregateFun = [&](const BoundingBox& box) -> bool {
        //pass one
        PixelIterator iterOut(outputRaster, box);
        PixelIterator iterIn(_inputObj.as<RasterCoverage>());
        PixelIterator iterEnd = iterOut.end();
        while(iterOut != iterEnd) {
            double v = numberer.value(iterIn) ;
           *iterOut = v;
            ++iterOut;
            ++iterIn;
            updateTranquilizer(currentCount++, 1000);
        }
        //pass two
        for(auto iterPass2 : numberer.pass2Entries()) {
            for(double& v : _outputObj.as<RasterCoverage>()){
                if ( v == iterPass2.first){
                    v = iterPass2.second;
                }
            }
        }

        return true;
    };
    ctx->_threaded = false; // operation can not be run in parallel
    bool res = OperationHelperRaster::execute(ctx, aggregateFun, outputRaster);

    INamedIdDomain iddom = outputRaster->datadef().domain<>().as<NamedIdDomain>();
    NamedIdentifierRange range;
    for(int i=0; i < numberer.lastid(); ++i) {
        range << QString("area_%1").arg(i);
    }
    iddom->setRange(range);

    QVariant value;
    value.setValue<IRasterCoverage>(outputRaster);
    logOperation(outputRaster,_expression);
    ctx->setOutput(symTable,value,outputRaster->name(), itRASTER, outputRaster->resource() );
    return res;
}

Ilwis::OperationImplementation::State AreaNumbering::prepare(ExecutionContext *ctx, const SymbolTable &st )
{
    OperationImplementation::prepare(ctx,st);
    QString raster = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();
    int copylist = itCOORDSYSTEM | itGEOREF;

    if (!_inputObj.prepare(raster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
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
        ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
        return sPREPAREFAILED;
    }

    IRasterCoverage outputRaster = _outputObj.as<RasterCoverage>();
    if ( outputName != sUNDEF)
        _outputObj->name(outputName);

    QString outputBaseName = outputName;
    int index = 0;
    if ( (index = outputName.lastIndexOf(".")) != -1) {
        outputBaseName = outputName.left(index);
    }

    IDomain dom;
    QString domname = QString(INTERNAL_CATALOG + "/%1").arg(outputBaseName);
    Resource resource(QUrl(domname), itITEMDOMAIN );
    resource.setExtendedType(itNAMEDITEM);
    if (!dom.prepare(resource))
        return sPREPAREFAILED;
    outputRaster->datadefRef().domain(dom);
    mastercatalog()->addItems({resource});

    initialize(outputRaster->size().linearSize());

    return sPREPARED;
}

quint64 AreaNumbering::createMetadata()
{
    OperationResource operation({"ilwis://operations/areanumbering"});
    operation.setLongName("Area Numbering");
    operation.setSyntax("areanumbering(inputgridcoverage,connectivity=!4|8)");
    operation.setDescription(TR("Area numbering assigns unique pixel values in an output map for connected areas (areas consisting of pixels with the same value, class name, or ID)"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itRASTER , TR("input rastercoverage"),TR("rastercoverage with domain item, boolean or identifier domain"));
    operation.addInParameter(1,itUINT8 , TR("Connectivity"),TR("Connected cells, maybe 4 or 8"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output rastercoverage"),TR("output rastercoverage with the identifier domain"));
    operation.addOutParameter(1,itRASTER, TR("output raster"),TR("output rastercoverage with the identifier domain"));
    operation.setKeywords("raster,classification");

    mastercatalog()->addItems({operation});
    return operation.id();
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
    qint32 x = in.position().x;
    qint32 y = in.position().y;
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


