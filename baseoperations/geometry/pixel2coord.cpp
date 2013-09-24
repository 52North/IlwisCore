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
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    Coordinate crd = _inputGC->georeference()->pixel2Coord(Pixel_d(_voxel.x(), _voxel.y()));
    QVariant var;
    var.setValue<Coordinate>(crd);
    ctx->addOutput(symTable,var,_outName,itCOORD3D, Resource());

    return true;
}

OperationImplementation *Pixel2Coord::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Pixel2Coord(metaid, expr);
}

Ilwis::OperationImplementation::State Pixel2Coord::prepare(ExecutionContext *ctx, const SymbolTable& symTable)
{
    QString raster = _expression.parm(0).value();
    if (!_inputGC.prepare(raster)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
        return sPREPAREFAILED;
    }
    if ( _expression.parameterCount() == 2) {
        QString name = _expression.parm(1).value();
        QVariant var = symTable.getValue(name);
        _voxel = var.value<Voxel>();
    }
    if ( _expression.parameterCount() == 3) {
        bool ok1, ok2, ok3=true;
        qint32 x = _expression.parm(1).value().toInt(&ok1);
        qint32 y = _expression.parm(2).value().toInt(&ok2);
        qint32 z = 0;
        if(_expression.parameterCount() == 4) {
            z  = _expression.parm(2).value().toInt(&ok3);
        }
        if (! (ok1 && ok2 && ok3)) {
            ERROR2(ERR_ILLEGAL_VALUE_2,"Pixel", QString("%1 %2 %3").arg(x,y,x));
            return sPREPAREFAILED;
        }
        _voxel = Voxel(x,y,z)    ;
    }
    if ( _expression.parameterCount(false) == 1) {
        _outName = _expression.parm(0,true,false).value();
    }
    if ( _voxel.isValid())
        return sPREPARED;

    return sPREPAREFAILED;
}

quint64 Pixel2Coord::createMetadata()
{
    QString url = QString("ilwis://operations/pixel2coord");
    Resource resource(QUrl(url), itOPERATIONMETADATA);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","pixel2coord");
    resource.addProperty("syntax","pixel2coord(rastercoverage|georef,Pixel)");
    resource.addProperty("description",TR("translates a pixel to a coordinate location based on the supplied georeference"));
    resource.addProperty("inparameters","2");
    resource.addProperty("pin_1_type", itRASTER | itGEOREF);
    resource.addProperty("pin_1_name", TR("input rastercoverage or georeference"));
    resource.addProperty("pin_1_desc",TR("input rastercoverage with domain any domain or georefence"));
    resource.addProperty("pin_2_type", itVOXEL);
    resource.addProperty("pin_2_name", TR("source pixel"));
    resource.addProperty("pin_2_desc",TR("the pixel that has to translted to a world coordinate"));
    resource.addProperty("outparameters",1);
    resource.addProperty("pout_1_type", itCOORD3D);
    resource.addProperty("pout_1_name", TR("2D coordinate"));
    resource.addProperty("pout_1_desc",TR("2D coordinate"));
    resource.prepare();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();
}
