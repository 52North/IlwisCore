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
#include "identity.h"
#include "OperationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "commandhandler.h"
#include "pixeliterator.h"
#include "operation.h"
#include "operationhelper.h"
#include "sinus.h"

using namespace Ilwis;
using namespace BaseOperations;

Sinus::Sinus() {

}

Sinus::Sinus(quint64 metaid,const Ilwis::OperationExpression& expr) : OperationImplementation(metaid, expr),_spatialCase(true), _number(rUNDEF)
{

}

bool Sinus::execute(ExecutionContext *ctx)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare()) != sPREPARED)
            return false;

    QVariant value;
    if ( _spatialCase) {
        PixelIterator iterIn(_inputGC, _box);
        PixelIterator iterOut(_outputGC, Box3D<qint32>(_box.size()));

        double v_in = 0;
        for_each(iterOut, iterOut.end(), [&](double& v){
            if ( (v_in = *iterIn) != rUNDEF) {
                v = sin(v_in);
            }
            ++iterIn;
        });

        value.setValue<IGridCoverage>(_outputGC);


    } else {
        double v = sin(_number);
        value.setValue<double>(v);
    }
    if ( ctx && value.isValid())
        ctx->_results.push_back(value);

    return true;
}

OperationImplementation *Sinus::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Sinus(metaid,expr);
}

OperationImplementation::State Sinus::prepare()
{
    IlwisTypes ptype = _expression.parm(0).valuetype();


    if ( ptype & itNUMERIC ) {
        _spatialCase = false;
        bool ok;
        _number = _expression.parm(0).value().toDouble(&ok);
        if (!ok) {
            ERROR2(ERR_NO_OBJECT_TYPE_FOR_2,"Numerical value", "sinus operation");
            _number = rUNDEF;
            return sPREPAREFAILED;
        }
        return sPREPARED;

    } else if ( ptype == itGRIDCOVERAGE) {
        QString gc = _expression.parm(0).value();
        QString outputName = _expression.parm(0,false).value();

        if (!_inputGC.prepare(gc)) {
            ERROR2(ERR_COULD_NOT_LOAD_2,gc,"");
            return sPREPAREFAILED;
        }
        OperationHelper helper;
        _box = helper.initialize(_inputGC, _outputGC, _expression.parm(0),
                                    itGRIDSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);
        if ( !_outputGC.isValid()) {
            ERROR1(ERR_NO_INITIALIZED_1, "output gridcoverage");
            return sPREPAREFAILED;
        }
        IDomain dom;
        if(!dom.prepare("min1to1"))
            return sPREPAREFAILED;

        _outputGC->setDomain(dom);
        if ( outputName != sUNDEF)
            _outputGC->setName(outputName);
        _spatialCase = true;
        return sPREPARED;
    }
    return sNOTPREPARED;
}

quint64 Sinus::createMetadata() {
    QString url = QString("ilwis://operations/sin");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","sinus");
    res.addProperty("inparameters","1");
    res.addProperty("pin_1_type", itGRIDCOVERAGE);
    res.addProperty("pin_1_name", TR("input gridcoverage"));
    res.addProperty("pin_1_domain","value");
    res.addProperty("pin_1_desc",TR("input gridcoverage with a numerical domain; values are considered to be radians"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itGRIDCOVERAGE);
    res.addProperty("pout_1_name", TR("output gridcoverage"));
    res.addProperty("pout_1_domain","min1to1");
    res.addProperty("pout_1_desc",TR("output gridcoverage with a numerical domain; values are between -1 and 1"));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();
}


