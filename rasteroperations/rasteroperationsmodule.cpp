#include <QtPlugin>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include "kernel.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "rasteroperationsmodule.h"
#include "ilwisdata.h"
#include "raster.h"
#include "domain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "pixeliterator.h"
#include "aggregateraster.h"
#include "domainitem.h"
#include "identifieritem.h"
#include "itemrange.h"
#include "identifierrange.h"
#include "itemdomain.h"
#include "crossrasters.h"
#include "rasterstrechoperation.h"

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
   commandhandler()->addOperation(LinearStretchOperation::createMetadata(), LinearStretchOperation::create);

  kernel()->issues()->log("Loaded raster operations module",IssueObject::itMessage);

}

QString RasterOperationsModule::name() const
{
    return "Raster Operations plugin";
}

QString RasterOperationsModule::version() const
{
    return "1.0";
}






