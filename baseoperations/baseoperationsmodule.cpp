#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
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
#include "data/selection.h"
#include "geometry/fcoordinate.h"
#include "geometry/fpixel.h"
#include "geometry/pixel2coord.h"
#include "geometry/coord2pixel.h"
#include "util/text2output.h"

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
    commandhandler()->addOperation(Sine::createMetadata(), Sine::create);
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
    commandhandler()->addOperation(Selection::createMetadata(), Selection::create);
    commandhandler()->addOperation(FCoordinate::createMetadata(), FCoordinate::create);
    commandhandler()->addOperation(FPixel::createMetadata(), FPixel::create);
    commandhandler()->addOperation(Pixel2Coord::createMetadata(), Pixel2Coord::create);
    commandhandler()->addOperation(Coord2Pixel::createMetadata(), Coord2Pixel::create);
    commandhandler()->addOperation(Text2Output::createMetadata(), Text2Output::create);
}

QString RasterOperationsModule::name() const
{
    return "Base Operations plugin";
}

QString RasterOperationsModule::version() const
{
    return "1.0";
}
