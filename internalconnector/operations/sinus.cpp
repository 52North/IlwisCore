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

Sinus::Sinus() {

}

Sinus::Sinus(quint64 metaid,const Ilwis::OperationExpression& expr) : OperationImplementation(metaid, expr)
{

}

bool Sinus::execute(ExecutionContext *ctx)
{

    IlwisTypes ptype = _expression.parm(0).valuetype();
    QString gc = _expression.parm(0).value();

    if ( ptype & itNUMERIC ) {

        return true;

    } else if ( ptype == itGRIDCOVERAGE) {

        IGridCoverage inputGC;
        if (!inputGC.prepare(gc)) {
            return ERROR2(ERR_COULD_NOT_LOAD_2,gc,"");
        }
        IGridCoverage gcnew;
        OperationHelper helper;
        Box3D<qint32> box = helper.initialize(inputGC, gcnew, _expression.parm(0),
                                    itGRIDSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);
        if ( !gcnew.isValid()) {
            return ERROR1(ERR_NO_INITIALIZED_1, "output gridcoverage");
        }
        IDomain dom;
        dom.prepare("min1to1");
        gcnew->setDomain(dom);

        PixelIterator iterIn(inputGC, box);
        PixelIterator iterOut(gcnew, Box3D<qint32>(box.size()));

        double v_in = 0;
        for_each(iterOut, iterOut.end(), [&](double& v){
            if ( (v_in = *iterIn) != rUNDEF) {
                v = sin(v_in);
            }
            ++iterIn;
        });


        if ( ctx) {
            QVariant value;
            value.setValue<IGridCoverage>(gcnew);

            ctx->_results.push_back(value);
        }
    }

    return true;
}

OperationImplementation *Sinus::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Sinus(metaid,expr);
}

quint64 Sinus::createMetadata() {
    QString url = QString("ilwis://operations/sin").arg(itGRIDCOVERAGE).arg(itGRIDCOVERAGE);
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","sinus");
    res.addProperty("inparameters",1);
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

    IOperationMetaData md;
    if(!md.prepare(res)) {
        return i64UNDEF;
    }

    return md->id();
}


