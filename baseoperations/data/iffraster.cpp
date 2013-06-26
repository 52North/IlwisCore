#include <QString>
#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "pixeliterator.h"
#include "ilwisoperation.h"
#include "iffraster.h"

using namespace Ilwis;
using namespace BaseOperations;

IffRaster::IffRaster()
{
}


IffRaster::IffRaster(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
    _number[0] = _number[1] = rUNDEF;
}

bool IffRaster::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    BoxedAsyncFunc iffunc = [&](const Box3D<qint32>& box) -> bool {

        PixelIterator iterOut(_outputGC,box);
        PixelIterator iterIn(_inputGC,box);
        PixelIterator iter1, iter2;
        bool isCoverage1 = _coverages[0].isValid();
        bool isCoverage2 = _coverages[1].isValid();
        if ( isCoverage1)
            iter1 = PixelIterator(_coverages[0], box);
        if ( isCoverage2)
            iter2 = PixelIterator(_coverages[1], box);

        while(iterOut != iterOut.end()) {
            double v1,v2;
            if ( isCoverage1) {
                v1 = *iter1;
                ++iter1;
            }
            if ( isCoverage2) {
                v2 = *iter2;
                ++iter2;
            }
            if (_number[0] != rUNDEF)
                v1 = _number[0];
            if ( _number[1] != rUNDEF)
                v2 = _number[1];

            *iterOut = *iterIn ? v1 : v2;

            ++iterOut;
            ++iterIn;
        }
        return true;

    };

    bool res = OperationHelper::execute(ctx, iffunc, _outputGC);

    if ( res && ctx != 0) {
        QVariant value;
        value.setValue<IGridCoverage>(_outputGC);
        ctx->addOutput(symTable,value,_outputGC->name(),itGRIDCOVERAGE,_outputGC->source());
    }
    return res;
}

OperationImplementation *IffRaster::create(quint64 metaid, const OperationExpression &expr)
{
    return new IffRaster(metaid, expr);
}

OperationImplementation::State IffRaster::prepare(ExecutionContext *, const SymbolTable &)
{
    QString gc = _expression.parm(0).value();

    if (!_inputGC.prepare(gc)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,gc,"");
        return sPREPAREFAILED;
    }
    DataDefinition outputDataDef = findOutputDataDef(_expression);


    OperationHelper helper;
    helper.initialize(_inputGC, _outputGC, _expression.parm(0),
                                itGRIDSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);

    _outputGC->datadef() = outputDataDef;

    return sPREPARED;
}

DataDefinition IffRaster::findOutputDataDef(const OperationExpression &expr ) {
    IDomain dm;
    QString domName = expr.parm(0,false).domain();
    if ( domName != sUNDEF && domName != "") {
        if( dm.prepare(domName))
            return DataDefinition(dm);
    }
    DataDefinition def1 = findParameterDataDef(expr,1);
    DataDefinition def2 = findParameterDataDef(expr,2);

    return DataDefinition::merge(def1,def2);
}

DataDefinition IffRaster::findParameterDataDef(const OperationExpression &expr, int index)  {
    const Parameter& parm = expr.parm(index);
    DataDefinition def;
    QString parmvalue = parm.value().toLower();

    quint64 gcid = mastercatalog()->name2id(parmvalue, itGRIDCOVERAGE);
    if ( gcid != i64UNDEF) {
        IGridCoverage gc;
        if(gc.prepare(gcid)) {
            def = gc->datadef();
            _coverages[index - 1] = gc;
        }
    } else {
        bool ok;
        _number[index - 1] =parmvalue.toDouble(&ok);
        if ( ok){
            def.domain().prepare("value");
        } else {
            std::vector<QString> bools = {"true","false","yes","no","?"};
            auto iter = std::find(bools.begin(), bools.end(), parmvalue.toLower());
            if ( iter != bools.end()) {
                int v = 0;
                if ( parmvalue == "?" )
                    v= 2;
                else if ( parmvalue == "yes" || parmvalue == "true")
                    v = 1;
                _number[index - 1] = v;
                IDomain dm;
                dm.prepare("boolean");
                def.domain(dm);
            }
        }
    }
    return def;
}

quint64 IffRaster::createMetadata()
{
    QString url = QString("ilwis://operations/iff");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","iff");
    res.addProperty("syntax","iffraster(gridcoverage,outputchoicetrue, outputchoicefalse)");
    res.addProperty("inparameters","3");
    res.addProperty("pin_1_type", itGRIDCOVERAGE);
    res.addProperty("pin_1_name", TR("input gridcoverage"));
    res.addProperty("pin_1_desc",TR("input gridcoverage with boolean domain"));
    res.addProperty("pin_2_type", itNUMERIC | itSTRING | itBOOL | itGRIDCOVERAGE);
    res.addProperty("pin_2_name", TR("true choice"));
    res.addProperty("pin_2_desc",TR("value returned when the boolean input pixel is true"));
    res.addProperty("pin_3_type", itNUMERIC | itSTRING | itBOOL | itGRIDCOVERAGE);
    res.addProperty("pin_3_name", TR("false choice"));
    res.addProperty("pin_3_desc",TR("value returned when the boolean input pixel is false"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itGRIDCOVERAGE);
    res.addProperty("pout_1_name", TR("gridcoverage"));
    res.addProperty("pout_1_desc",TR("gridcoverage with all pixels that correspond to the true value in the input having a value"));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();
}
