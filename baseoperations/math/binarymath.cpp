#include <QString>
#include <QVector>
#include <QSqlQuery>
#include <QSqlError>
#include <functional>

#include "kernel.h"
#include "ilwis.h"
#include "angle.h"
#include "point.h"
#include "box.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "domain.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "coordinatesystem.h"
#include "valuedefiner.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "columndefinition.h"
#include "table.h"
#include "containerstatistics.h"
#include "coverage.h"
#include "georeference.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "grid.h"
#include "gridcoverage.h"
#include "OperationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "operationhelper.h"
#include "commandhandler.h"
#include "pixeliterator.h"
#include "binarymath.h"

using namespace Ilwis;


OperationImplementation *BinaryMath::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new BinaryMath( metaid, expr);
}

BinaryMath::BinaryMath() : _coveragecoverage(false)
{
}

BinaryMath::BinaryMath(quint64 metaid,const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr) , _coveragecoverage(false)
{
}

bool BinaryMath::setOutput(ExecutionContext *ctx) {
    if ( ctx) {
        QVariant value;
        value.setValue<IGridCoverage>(_outputGC);

        ctx->_results.push_back(value);
    }
    return _outputGC.isValid();
}

bool BinaryMath::executeCoverageNumber(ExecutionContext *ctx) {
    PixelIterator iterIn(_inputGC1, _box);
    PixelIterator iterOut(_outputGC, Box3D<qint32>(_box.size()));

    double v_in = 0;
    for_each(iterOut, iterOut.end(), [&](double& v){
        if ( (v_in = *iterIn) != rUNDEF) {
            switch(_operator) {
                case otPLUS:
                   v = v_in + _number;break;
                case otMINUS:
                   v = v_in - _number;break;
                case otDIV:
                if ( _number != 0)
                    v = v_in / _number;
                else
                    v = rUNDEF;
                break;
                case otMULT:
                    v = v_in * _number;break;
            }
         }
        ++iterIn;
    });

    return setOutput(ctx);

}

bool BinaryMath::executeCoverageCoverage(ExecutionContext *ctx) {
    PixelIterator iterIn1(_inputGC1, _box);
    PixelIterator iterIn2(_inputGC2, _box);
    PixelIterator iterOut(_outputGC, Box3D<qint32>(_box.size()));

    double v_in1 = 0;
    double v_in2 = 0;
    for_each(iterOut, iterOut.end(), [&](double& v){
        v_in1 = *iterIn1;
        v_in2 = *iterIn2;
        if ( v_in1 != rUNDEF && v_in2 != rUNDEF) {
            switch(_operator) {
                case otPLUS:
                   v = v_in1 + v_in2;break;
                case otMINUS:
                   v = v_in1 - v_in2;break;
                case otDIV:
                if ( v_in2 != 0)
                    v = v_in1 / v_in2;
                else
                    v = rUNDEF;
                break;
                case otMULT:
                    v = v_in1 * v_in2;break;
            }
        }
        ++iterIn1;
        ++iterIn2;
    });

    return setOutput(ctx);


}

bool BinaryMath::execute(ExecutionContext *ctx)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare()) != sPREPARED)
            return false;

    if ( _coveragecoverage) {
        return executeCoverageCoverage(ctx);

    } else  {
        return executeCoverageNumber(ctx);
    }
    return true;
}

bool BinaryMath::prepareCoverageCoverage() {
    QString gc =  _expression.parm(0).value();
    if (!_inputGC1.prepare(gc)) {
        kernel()->issues()->log(TR(ERR_COULD_NOT_LOAD_2).arg(gc, ""));
        return false;
    }
    gc =  _expression.parm(1).value();
    if (!_inputGC2.prepare(gc)) {
        kernel()->issues()->log(TR(ERR_COULD_NOT_LOAD_2).arg(gc, ""));
        return false;
    }
    bool isNumeric = _inputGC1->domain()->ilwisType() == itNUMERICDOMAIN && _inputGC2->domain()->ilwisType() == itNUMERICDOMAIN;
    if (!isNumeric)
        return false;

    OperationHelper helper;
    _box = helper.initialize(_inputGC1, _outputGC, _expression.parm(0),
                                itGRIDSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);

    auto nrange1 = _inputGC1->range().dynamicCast<NumericRange>();
    if (nrange1.isNull())
        return false;

    auto nrange2 = _inputGC2->range().dynamicCast<NumericRange>();
    if (nrange2.isNull())
        return false;

    double rmax, rmin;
    switch(_operator) {
        case otPLUS:
           rmin = nrange1->min() + nrange2->min();
           rmax = nrange1->max() + nrange1->max();
           break;
        case otMINUS:
            rmin = nrange1->min() - nrange2->min();
            rmax = nrange1->max() - nrange1->max();
            break;
        case otDIV:
            rmin = nrange2->min() != 0 ? nrange1->min() / nrange2->min() : std::min(nrange1->min(), nrange2->min());
            rmax = nrange2->max() != 0 ? nrange1->max() / nrange2->max() : std::min(nrange1->max(), nrange2->max());
            break;
        case otMULT:
            rmin = nrange1->min() * nrange2->min();
            rmax = nrange1->max() * nrange1->max();
            break;
    }
    NumericRange *newRange = new NumericRange(rmin,
                                              rmax,
                                              std::min(nrange1->step(), nrange2->step()));

    IDomain dom;
    dom.prepare("value");
    _outputGC->setDomain(dom);
    _outputGC->setRange(newRange);

    _coveragecoverage = true;
    return true;
}

bool BinaryMath::prepareCoverageNumber(IlwisTypes ptype1, IlwisTypes ptype2) {

    int mindex = (ptype1 & itNUMERIC) == 0 ? 0 : 1;
    int nindex = mindex ? 0 : 1;

    QString gc =  _expression.parm(mindex).value();
    if (!_inputGC1.prepare(gc)) {
        kernel()->issues()->log(TR(ERR_COULD_NOT_LOAD_2).arg(gc, ""));
        return false;
    }
    if(_inputGC1->domain()->ilwisType() != itNUMERICDOMAIN)
        return false;

    _number = _expression.parm(nindex).value().toDouble();

    OperationHelper helper;
    _box = helper.initialize(_inputGC1, _outputGC, _expression.parm(mindex),
                                itGRIDSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);

    auto nrange = _inputGC1->range().dynamicCast<NumericRange>();
    if (nrange.isNull())
        return false;
    double rmax, rmin;
    switch(_operator) {
        case otPLUS:
           rmin = nrange->min() + _number;
           rmax = nrange->max() + _number;
           break;
        case otMINUS:
            rmin = nrange->min() - _number;
            rmax = nrange->max() - _number;
            break;
        case otDIV:
            rmin = _number != 0 ? nrange->min() / _number : nrange->min();
            rmax = _number != 0 ? nrange->max() / _number : nrange->max();
            break;
        case otMULT:
            rmin = nrange->min() * _number;
            rmax = nrange->max() * _number;
            break;
    }
    NumericRange *newRange = new NumericRange(rmin,
                                              rmax,
                                              nrange->step());

    IDomain dom;
    dom.prepare("value");
    _outputGC->setDomain(dom);
    _outputGC->setRange(newRange);

    return true;
}

OperationImplementation::State BinaryMath::prepare() {
    if ( _expression.parameterCount() != 3){
        return sPREPAREFAILED;
    }

    IlwisTypes ptype1 = _expression.parm(0).valuetype();
    IlwisTypes ptype2 = _expression.parm(1).valuetype();

    QString oper = _expression.parm(2).value();
    if ( oper.toLower() == "add")
        _operator = otPLUS;
    else if ( oper.toLower() == "substract")
        _operator = otMINUS;
    else if ( oper == "divide")
        _operator = otDIV;
    else
        _operator = otMULT;

    if ( ((ptype1 | ptype2) & (itGRIDCOVERAGE | itNUMERIC)) ) {
        if(!prepareCoverageNumber(ptype1, ptype2))
            return sPREPAREFAILED;

    } else if ( ptype1 & ptype2 & itGRIDCOVERAGE ) {
        if(!prepareCoverageCoverage())
            return sPREPAREFAILED;
    }

    return sPREPARED;
}

quint64 BinaryMath::createMetadata()
{
    QString url = QString("ilwis://operations/binarymathraster");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","binarymathRaster");
    res.addProperty("inparameters",3);
    res.addProperty("pin_1_type", itGRIDCOVERAGE | itNUMERIC);
    res.addProperty("pin_1_name", TR("input gridcoverage or number"));
    res.addProperty("pin_1_domain","value");
    res.addProperty("pin_1_desc",TR("input gridcoverage with a numerical domain or number"));
    res.addProperty("pin_2_type", itGRIDCOVERAGE | itNUMERIC);
    res.addProperty("pin_2_name", TR("input gridcoverage or number"));
    res.addProperty("pin_2_domain","value");
    res.addProperty("pin_2_desc",TR("input gridcoverage with a numerical domain or number"));
    res.addProperty("pin_3_type", itSTRING);
    res.addProperty("pin_3_name", TR("Operator"));
    res.addProperty("pin_3_domain","string");
    res.addProperty("pin_3_desc",TR("operator (add, substract,divide, multiply) applied to the other 2 input operators"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itGRIDCOVERAGE);
    res.addProperty("pout_1_name", TR("output gridcoverage"));
    res.addProperty("pout_1_domain","value");
    res.addProperty("pout_1_desc",TR("output gridcoverage with a numerical domain"));

    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();
}
