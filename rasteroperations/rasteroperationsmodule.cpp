#include <QtPlugin>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include "kernel.h"
#include "symboltable.h"
#include "OperationExpression.h"
#include "rasteroperationsmodule.h"
#include "ilwisdata.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "commandhandler.h"
#include "raster.h"
#include "pixeliterator.h"
#include "aggregateraster.h"
#include "areanumbering.h"

using namespace Ilwis;
using namespace RasterOperations;

RasterOperationsModule::RasterOperationsModule(QObject *parent) :
    Module(parent, "RasterOperations", "iv40","1.0")
{
}

QString RasterOperationsModule::getInterfaceVersion() const
{
    return "iv40";

}

void RasterOperationsModule::prepare()
{
   commandhandler()->addOperation(AggregateRaster::createMetadata(), AggregateRaster::create);
   commandhandler()->addOperation(AreaNumbering::createMetadata(), AreaNumbering::create);

}

QString RasterOperationsModule::name() const
{
    return "Raster Operations plugin";
}

QString RasterOperationsModule::version() const
{
    return "1.0";
}






