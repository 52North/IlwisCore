#include "baseoperationsmodule.h"
#include "ilwisdata.h"
#include "angle.h"
#include "point.h"
#include "range.h"
#include "box.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "ilwisobjectfactory.h"
#include "ilwisobjectconnector.h"
#include "size.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "domain.h"
#include "valuedefiner.h"
#include "columndefinition.h"
#include "table.h"
#include "containerstatistics.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "coordinatesystem.h"
#include "coverage.h"
#include "georeference.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "grid.h"
#include "gridcoverage.h"
#include "commandhandler.h"
#include "math/sinus.h"
#include "math/binarymath.h"
#include "geometry/resampleraster.h"
#include "data/rasvalue.h"

using namespace Ilwis;
using namespace BaseOperations;

RasterOperationsModule::RasterOperationsModule(QObject *parent) : Module(parent, "RasterOperations", "iv40","1.0")
{
}

QString RasterOperationsModule::getInterfaceVersion() const
{
    return "iv40";

}

void RasterOperationsModule::prepare()
{
    commandhandler()->addOperation(Sinus::createMetadata(), Sinus::create);
    commandhandler()->addOperation(BinaryMath::createMetadata(), BinaryMath::create);
    commandhandler()->addOperation(ResampleRaster::createMetadata(), ResampleRaster::create);
    commandhandler()->addOperation(RasValue::createMetadata(), RasValue::create);
}

QString RasterOperationsModule::name() const
{
    return "Raster Operations plugin";
}

QString RasterOperationsModule::version() const
{
    return "1.0";
}
