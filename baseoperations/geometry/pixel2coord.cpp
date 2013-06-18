#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "pixel2coord.h"

using namespace Ilwis;

Pixel2Coord::Pixel2Coord()
{
}

Pixel2Coord::Pixel2Coord(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool Pixel2Coord::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx)) != sPREPARED)
            return false;
}

OperationImplementation *Pixel2Coord::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Pixel2Coord(metaid, expr);
}

Ilwis::OperationImplementation::State Pixel2Coord::prepare(ExecutionContext *ctx)
{
    //if (!ctx || ctx->_results.size() == 0)
        return sPREPAREFAILED;
    //Voxel vx = ctx->_results[0].value<Voxel>();

}

quint64 Pixel2Coord::createMetadata()
{
    QString url = QString("ilwis://operations/pixel2coord");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","pixel2coord");
    res.addProperty("syntax","pixel2coord(gridcoverage|georef,Pixel)");
    res.addProperty("inparameters","2");
    res.addProperty("pin_1_type", itGRIDCOVERAGE | itGEOREF);
    res.addProperty("pin_1_name", TR("input gridcoverage or georeference"));
    res.addProperty("pin_1_desc",TR("input gridcoverage with domain any domain or georefence"));
    res.addProperty("pin_2_type", itPIXEL);
    res.addProperty("pin_2_name", TR("source pixel"));
    res.addProperty("pin_2_desc",TR("the pixel that has to translted to a world coordinate"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itCOORD2D);
    res.addProperty("pout_1_name", TR("2D coordinate"));
    res.addProperty("pout_1_desc",TR("2D coordinate"));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();
}
