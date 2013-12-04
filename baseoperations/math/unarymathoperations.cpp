#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "pixeliterator.h"
#include "unarymath.h"
#include "unarymathoperations.h"

using namespace Ilwis;
using namespace BaseOperations;

Sine::Sine(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMath(metaid, expr, "min1to1", sin)
{}
OperationImplementation *Sine::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new Sine(metaid,expr);}

quint64 Sine::createMetadata() {
    Resource resource = UnaryMath::populateMetadata(QString("ilwis://operations/sin"), "sine", "min1to1");
    resource.addProperty("pin_1_desc", QString("input data source with a numerical domain; values are considered to be radians"));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain; values are between -1 and 1"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
Cosine::Cosine(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMath(metaid, expr, "min1to1", sin)
{}
OperationImplementation *Cosine::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new Cosine(metaid,expr);}

quint64 Cosine::createMetadata() {
    Resource resource = UnaryMath::populateMetadata(QString("ilwis://operations/cos"), "cosine", "min1to1");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain; values are considered to be radians"));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain; values are between -1 and 1"));

    mastercatalog()->addItems({resource});
    return resource.id();
}

//----------------------------------------------------------
double tangent(double v){
    if ( std::abs(v) == M_PI / 2)
        return rUNDEF;
    return tan(v);
}

Tangent::Tangent(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMath(metaid, expr, "value", tangent)
{}
OperationImplementation *Tangent::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new Tangent(metaid,expr);}

quint64 Tangent::createMetadata() {
    Resource resource = UnaryMath::populateMetadata(QString("ilwis://operations/tan"), "Tangent", "value");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain; values are considered to be radians"));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain; "));

    mastercatalog()->addItems({resource});
    return resource.id();
}

//----------------------------------------------------------
double arcsine(double v){
    if ( v < -1 || v > 1)
        return rUNDEF;
    return asin(v);
}
Arcsine::Arcsine(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMath(metaid, expr, "value", arcsine)
{}
OperationImplementation *Arcsine::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new Arcsine(metaid,expr);}

quint64 Arcsine::createMetadata() {
    Resource resource = UnaryMath::populateMetadata(QString("ilwis://operations/asin"), "Arcsine", "value");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain;with value between -1 and 1 "));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain; values are between pi/2 and -pi/2"));

    mastercatalog()->addItems({resource});
    return resource.id();
}

//----------------------------------------------------------
double arccosine(double v){
    if ( v < -1 || v > 1)
        return rUNDEF;
    return asin(v);
}
Arccosine::Arccosine(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMath(metaid, expr, "value", arccosine)
{}
OperationImplementation *Arccosine::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new Arccosine(metaid,expr);}

quint64 Arccosine::createMetadata() {
    Resource resource = UnaryMath::populateMetadata(QString("ilwis://operations/acos"), "Arccosine", "value");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain with value between -1 and 1; "));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain; values are between 0 and pi"));

    mastercatalog()->addItems({resource});
    return resource.id();
}

//----------------------------------------------------------
ArcTangent::ArcTangent(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMath(metaid, expr, "value", atan)
{}
OperationImplementation *ArcTangent::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new ArcTangent(metaid,expr);}

quint64 ArcTangent::createMetadata() {
    Resource resource = UnaryMath::populateMetadata(QString("ilwis://operations/atan"), "ArcTangent", "value");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain;with value between -pi/2 and pi/2 "));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain; values are between -1 and 1"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
double logten(double v){
    if ( v > 0)
        return log10(v);
    return rUNDEF;
}
Log10::Log10(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMath(metaid, expr, "value", logten)
{}
OperationImplementation *Log10::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new Log10(metaid,expr);}

quint64 Log10::createMetadata() {
    Resource resource = UnaryMath::populateMetadata(QString("ilwis://operations/log10"), "Log10", "value");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain;with value larger than zero "));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
double naturallog(double v){
    if ( v > 0)
        return log(v);
    return rUNDEF;
}
NaturalLogarithm::NaturalLogarithm(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMath(metaid, expr, "value", naturallog)
{}
OperationImplementation *NaturalLogarithm::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new NaturalLogarithm(metaid,expr);}

quint64 NaturalLogarithm::createMetadata() {
    Resource resource = UnaryMath::populateMetadata(QString("ilwis://operations/ln"), "Natural Logarithm", "value");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain;with value larger than zero "));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------

double abs2(double v){
    if ( v == rUNDEF)
        return rUNDEF;
    return v;
}
Abs::Abs(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMath(metaid, expr, "value", abs2)
{}
OperationImplementation *Abs::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new Abs(metaid,expr);}

quint64 Abs::createMetadata() {
    Resource resource = UnaryMath::populateMetadata(QString("ilwis://operations/abs"), "Absolute value", "value");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain"));
    resource.addProperty("pout_1_desc",QString("output data source with a positive numerical domain"));

    mastercatalog()->addItems({resource});
    return resource.id();
}

//---------------------------------------------------------
//----------------------------------------------------------

double sqrt2(double v){
    if ( v < 0)
        return rUNDEF;
    return std::sqrt(v);
}
Sqrt::Sqrt(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMath(metaid, expr, "value", sqrt2)
{}
OperationImplementation *Sqrt::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new Sqrt(metaid,expr);}

quint64 Sqrt::createMetadata() {
    Resource resource = UnaryMath::populateMetadata(QString("ilwis://operations/sqrt"), "Absolute value", "value");
    resource.addProperty("pin_1_desc",QString("input data source with a positive numerical domain (or 0)"));
    resource.addProperty("pout_1_desc",QString("output data source with a positive numerical domain"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
Ceil::Ceil(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMath(metaid, expr, "integer", std::ceil<int>)
{}
OperationImplementation *Ceil::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new Ceil(metaid,expr);}

quint64 Ceil::createMetadata() {
    Resource resource = UnaryMath::populateMetadata(QString("ilwis://operations/ceil"), "Ceiling value", "integer");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain"));
    resource.addProperty("pout_1_desc",QString("output data source with a integer numerical domain"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
Floor::Floor(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMath(metaid, expr, "integer", std::floor<int>)
{}
OperationImplementation *Floor::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new Floor(metaid,expr);}

quint64 Floor::createMetadata() {
    Resource resource = UnaryMath::populateMetadata(QString("ilwis://operations/floor"), "Floor value", "integer");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain"));
    resource.addProperty("pout_1_desc",QString("output data source with a integer numerical domain"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
double sign(double v) {
    if ( v < 0)
        return -1;
    if ( v > 0)
        return 1;
    return 0;
}

Sign::Sign(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMath(metaid, expr, "integer", sign)
{}
OperationImplementation *Sign::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new Floor(metaid,expr);}

quint64 Sign::createMetadata() {
    Resource resource = UnaryMath::populateMetadata(QString("ilwis://operations/sgn"), "Sign", "integer");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain"));
    resource.addProperty("pout_1_desc",QString("output data source with values -1,0,1 depending on the sign of the input value"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
CosineH::CosineH(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMath(metaid, expr, "value", cosh)
{}
OperationImplementation *CosineH::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new CosineH(metaid,expr);}

quint64 CosineH::createMetadata() {
    Resource resource = UnaryMath::populateMetadata(QString("ilwis://operations/cosh"), "Hyperbolic cosine", "value");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain; values are considered to be radians"));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
SineH::SineH(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMath(metaid, expr, "value", sinh)
{}
OperationImplementation *SineH::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new SineH(metaid,expr);}

quint64 SineH::createMetadata() {
    Resource resource = UnaryMath::populateMetadata(QString("ilwis://operations/sinh"), "Hyperbolic sine", "value");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain; values are considered to be radians"));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
