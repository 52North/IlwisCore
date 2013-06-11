#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "ilwisoperation.h"
#include "pixeliterator.h"
#include "baseoperationsmodule.h"
#include "math/unarymath.h"
#include "math/unarymathoperations.h"
#include "math/binarymath.h"
#include "math/binarylogical.h"
#include "geometry/resampleraster.h"
#include "data/rasvalue.h"
#include "data/iffraster.h"
#include "data/assignment.h"

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
    commandhandler()->addOperation(Cosine::createMetadata(), Cosine::create);
    commandhandler()->addOperation(Tangent::createMetadata(), Tangent::create);
    commandhandler()->addOperation(Arcsine::createMetadata(), Arcsine::create);
    commandhandler()->addOperation(Arccosine::createMetadata(), Arccosine::create);
    commandhandler()->addOperation(ArcTangent::createMetadata(), ArcTangent::create);
    commandhandler()->addOperation(Log10::createMetadata(), Log10::create);
    commandhandler()->addOperation(NaturalLogarithm::createMetadata(), NaturalLogarithm::create);
    commandhandler()->addOperation(Ceil::createMetadata(), Ceil::create);
    commandhandler()->addOperation(Floor::createMetadata(), Floor::create);
    commandhandler()->addOperation(Sign::createMetadata(), Sign::create);
    commandhandler()->addOperation(CosineH::createMetadata(), CosineH::create);
    commandhandler()->addOperation(SineH::createMetadata(), SineH::create);
    commandhandler()->addOperation(BinaryMath::createMetadata(), BinaryMath::create);
    commandhandler()->addOperation(BinaryLogical::createMetadata(), BinaryLogical::create);
    commandhandler()->addOperation(ResampleRaster::createMetadata(), ResampleRaster::create);
    commandhandler()->addOperation(RasValue::createMetadata(), RasValue::create);
    commandhandler()->addOperation(IffRaster::createMetadata(), IffRaster::create);
    commandhandler()->addOperation(Assignment::createMetadata(), Assignment::create);
}

QString RasterOperationsModule::name() const
{
    return "Raster Operations plugin";
}

QString RasterOperationsModule::version() const
{
    return "1.0";
}
