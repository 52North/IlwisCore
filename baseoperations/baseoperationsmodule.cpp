#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "gridinterpolator.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "baseoperationsmodule.h"
#include "data/rasvalue.h"
#include "data/ifoperation.h"
#include "data/iffraster.h"
#include "data/assignment.h"
#include "data/selection.h"
#include "data/selectionfeatures.h"
#include "geometry/resampleraster.h"
#include "geometry/fcoordinate.h"
#include "geometry/fpixel.h"
#include "geometry/pixel2coord.h"
#include "geometry/coord2pixel.h"
#include "geometry/gridsize.h"
#include "math/unarymath.h"
#include "math/unarymathoperations.h"
#include "math/binarymath.h"
#include "math/binarylogical.h"
#include "util/text2output.h"
#include "util/stringoperations.h"

using namespace Ilwis;
using namespace BaseOperations;

BaseOperationsModule::BaseOperationsModule(QObject *parent) : Module(parent, "BaseOperations", "iv40","1.0")
{
}

QString BaseOperationsModule::getInterfaceVersion() const
{
    return "iv40";

}

void BaseOperationsModule::prepare()
{
    commandhandler()->addOperation(Arccosine::createMetadata(), Arccosine::create);
    commandhandler()->addOperation(Arcsine::createMetadata(), Arcsine::create);
    commandhandler()->addOperation(ArcTangent::createMetadata(), ArcTangent::create);
    commandhandler()->addOperation(Assignment::createMetadata(), Assignment::create);
    commandhandler()->addOperation(BinaryLogical::createMetadata(), BinaryLogical::create);
    commandhandler()->addOperation(BinaryMath::createMetadata(), BinaryMath::create);
    commandhandler()->addOperation(Ceil::createMetadata(), Ceil::create);
    commandhandler()->addOperation(Coord2Pixel::createMetadata(), Coord2Pixel::create);
    commandhandler()->addOperation(Cosine::createMetadata(), Cosine::create);
    commandhandler()->addOperation(CosineH::createMetadata(), CosineH::create);
    commandhandler()->addOperation(Floor::createMetadata(), Floor::create);
    commandhandler()->addOperation(FCoordinate::createMetadata(), FCoordinate::create);
    commandhandler()->addOperation(FPixel::createMetadata(), FPixel::create);
    commandhandler()->addOperation(GridSize::createMetadata(), GridSize::create);
    commandhandler()->addOperation(IffRaster::createMetadata(), IffRaster::create);
    commandhandler()->addOperation(Log10::createMetadata(), Log10::create);
    commandhandler()->addOperation(NaturalLogarithm::createMetadata(), NaturalLogarithm::create);
    commandhandler()->addOperation(Pixel2Coord::createMetadata(), Pixel2Coord::create);
    commandhandler()->addOperation(RasValue::createMetadata(), RasValue::create);
    commandhandler()->addOperation(ResampleRaster::createMetadata(), ResampleRaster::create);
    commandhandler()->addOperation(Selection::createMetadata(), Selection::create);
    commandhandler()->addOperation(SelectionFeatures::createMetadata(), SelectionFeatures::create);
    commandhandler()->addOperation(Sign::createMetadata(), Sign::create);
    commandhandler()->addOperation(Sine::createMetadata(), Sine::create);
    commandhandler()->addOperation(SineH::createMetadata(), SineH::create);
    commandhandler()->addOperation(StringFind::createMetadata(), StringFind::create);
    commandhandler()->addOperation(StringSub::createMetadata(), StringSub::create);
    commandhandler()->addOperation(StringReplace::createMetadata(), StringReplace::create);
    commandhandler()->addOperation(Tangent::createMetadata(), Tangent::create);
    commandhandler()->addOperation(Text2Output::createMetadata(), Text2Output::create);

}

QString BaseOperationsModule::name() const
{
    return "Base Operations plugin";
}

QString BaseOperationsModule::version() const
{
    return "1.0";
}
