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
#include "boost/numeric/ublas/matrix.hpp"
#include "boostext.h"
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
#include "plus.h"

using namespace Ilwis;


OperationImplementation *Plus::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Plus( metaid, expr);
}

Plus::Plus()
{
}

Plus::Plus(quint64 metaid,const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool Plus::canExecute(const QString &command) const
{
    OperationExpression expr(command, otFunction) ;
    if ( expr.name() != "add")
        return false;

    if ( expr.parameterCount() != 2)
        return false;
    return true;
}

bool Plus::executeCoverageNumber(int indexCoverage, int indexNumber,ExecutionContext *ctx) {
    QString gc =  _expression.parm(indexCoverage).value();
    IGridCoverage inputGC;
    if (!inputGC.prepare(gc)) {
        kernel()->issues()->log(TR(ERR_COULD_NOT_LOAD_2).arg(gc, ""));
        return false;
    }
    double number = _expression.parm(indexNumber).value().toDouble();

    IGridCoverage gcnew;
    OperationHelper helper;
    Box3D<qint32> box = helper.initialize(inputGC, gcnew, _expression.parm(indexCoverage),
                                itGRIDSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);

    auto nrange = inputGC->range().dynamicCast<NumericRange>();
    if (nrange.isNull())
        return false;
    NumericRange *newRange = new NumericRange(nrange->min() + number, nrange->max() + number,nrange->step());
    IDomain dom;
    dom.prepare("value");
    gcnew->setDomain(dom);
    gcnew->setRange(newRange);

    PixelIterator iterIn(inputGC, box);
    PixelIterator iterOut(gcnew, Box3D<qint32>(box.size()));

    double v_in = 0;
    for_each(iterOut, iterOut.end(), [&](double& v){
        if ( (v_in = *iterIn) != rUNDEF) {
            v = v_in + number;
        }
        ++iterIn;
    });

    if ( ctx) {
        QVariant value;
        value.setValue<IGridCoverage>(gcnew);

        ctx->_results.push_back(value);
    }


    return gcnew.isValid();

}

bool Plus::executeCoverageCoverage() {
    QString gc1 = _expression.parm(0).value();
    IGridCoverage inputGC1;
    if (!inputGC1.prepare(gc1)) {
        kernel()->issues()->log(TR(ERR_COULD_NOT_LOAD_2).arg(gc1, ""));
        return false;
    }
    QString gc2 = _expression.parm(1).value();
    IGridCoverage inputGC2;
    if (!inputGC2.prepare(gc2)) {
        kernel()->issues()->log(TR(ERR_COULD_NOT_LOAD_2).arg(gc2, ""));
        return false;
    }
    return true;
}

bool Plus::execute(ExecutionContext *ctx)
{

    IlwisTypes ptype1 = _expression.parm(0).valuetype();
    IlwisTypes ptype2 = _expression.parm(1).valuetype();


    bool ok = false;
    if ( ((ptype1 | ptype2) & (itGRIDCOVERAGE | itNUMERIC)) ) {

        int mindex = (ptype1 & itNUMERIC) == 0 ? 0 : 1;
        int nindex = mindex ? 0 : 1;
        ok = executeCoverageNumber(mindex, nindex, ctx);

    } else if ( ptype1 & ptype2 & itGRIDCOVERAGE ) {
        ok = executeCoverageCoverage();
    }

    return ok;

}

quint64 Plus::createMetadata()
{
    QString url = QString("ilwis://operations/addmap").arg(itGRIDCOVERAGE).arg(itGRIDCOVERAGE);
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","addmap");
    res.addProperty("inparameters",1);
    res.addProperty("pin_1_type", itGRIDCOVERAGE | itNUMERIC);
    res.addProperty("pin_1_name", TR("input gridcoverage or number"));
    res.addProperty("pin_1_domain","value");
    res.addProperty("pin_1_desc",TR("input gridcoverage with a numerical domain or number"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itGRIDCOVERAGE);
    res.addProperty("pout_1_name", TR("output gridcoverage"));
    res.addProperty("pout_1_domain","value");
    res.addProperty("pout_1_desc",TR("output gridcoverage with a numerical domain"));

    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    IOperationMetaData md;
    if(!md.prepare(res)) {
        return i64UNDEF;
    }

    return md->id();
}
