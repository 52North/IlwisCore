#include <functional>
#include <future>
#include "raster.h"
#include "ilwisoperation.h"
#include "pixeliterator.h"
#include "baseoperationsmodule.h"
#include "math/sinus.h"
#include "math/binarymath.h"
#include "math/binarylogical.h"
#include "geometry/resampleraster.h"
#include "data/rasvalue.h"
#include "data/iffraster.h"

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
    commandhandler()->addOperation(BinaryLogical::createMetadata(), BinaryLogical::create);
    commandhandler()->addOperation(ResampleRaster::createMetadata(), ResampleRaster::create);
    commandhandler()->addOperation(RasValue::createMetadata(), RasValue::create);
    commandhandler()->addOperation(IffRaster::createMetadata(), IffRaster::create);
}

QString RasterOperationsModule::name() const
{
    return "Raster Operations plugin";
}

QString RasterOperationsModule::version() const
{
    return "1.0";
}
