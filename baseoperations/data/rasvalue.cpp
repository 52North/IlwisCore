#include <QString>
#include <QVector>
#include <QSqlQuery>
#include <QSqlError>
#include <functional>
#include <future>
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
#include "gridinterpolator.h"
//#include "identity.h"
#include "OperationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "commandhandler.h"
#include "pixeliterator.h"
#include "blockiterator.h"
#include "operation.h"
#include "operationhelper.h"
#include "rasvalue.h"

RasValue::RasValue()
{
}

RasValue::RasValue(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

bool RasValue::execute(ExecutionContext *ctx)
{
}

Ilwis::OperationImplementation *RasValue::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
        return new RasValue(metaid, expr);
}

Ilwis::OperationImplementation::State RasValue::prepare()
{
    QString gc = _expression.parm(0).value();
    if (!_inputGC.prepare(gc)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,gc,"");
        return sPREPAREFAILED;
    }


}

quint64 RasValue::createMetadata()
{
}
