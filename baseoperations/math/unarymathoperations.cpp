#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "pixeliterator.h"
#include "unarymath.h"
#include "unarymathrasterandnumber.h"
#include "unarymathtable.h"
#include "unarymathoperations.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(SineCoverage)

SineCoverage::SineCoverage(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathRasterAndNumber(metaid, expr,"min1to1", sin)
{}
OperationImplementation *SineCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new SineCoverage(metaid,expr);}

quint64 SineCoverage::createMetadata() {
    Resource resource = UnaryMathRasterAndNumber::populateMetadata(QString("ilwis://operations/sin"), "Sine for rasters");
    resource.addProperty("pin_1_desc", QString("input data source with a numerical domain; values are considered to be radians"));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain; values are between -1 and 1"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
REGISTER_OPERATION(CosineCoverage)
CosineCoverage::CosineCoverage(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathRasterAndNumber(metaid, expr,"min1to1", cos)
{}
OperationImplementation *CosineCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new CosineCoverage(metaid,expr);}

quint64 CosineCoverage::createMetadata() {
    Resource resource = UnaryMathRasterAndNumber::populateMetadata(QString("ilwis://operations/cos"), "Cosine  for rasters");
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

REGISTER_OPERATION(TangentCoverage)
TangentCoverage::TangentCoverage(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathRasterAndNumber(metaid, expr, "value",tangent)
{}
OperationImplementation *TangentCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new TangentCoverage(metaid,expr);}

quint64 TangentCoverage::createMetadata() {
    Resource resource = UnaryMathRasterAndNumber::populateMetadata(QString("ilwis://operations/tan"), "Tangent  for rasters");
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

REGISTER_OPERATION(ArcsineCoverage)

ArcsineCoverage::ArcsineCoverage(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathRasterAndNumber(metaid, expr, "value", arcsine)
{}
OperationImplementation *ArcsineCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new ArcsineCoverage(metaid,expr);}

quint64 ArcsineCoverage::createMetadata() {
    Resource resource = UnaryMathRasterAndNumber::populateMetadata(QString("ilwis://operations/asin"), "Arcsine  for rasters");
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

REGISTER_OPERATION(ArccosineCoverage)
ArccosineCoverage::ArccosineCoverage(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathRasterAndNumber(metaid, expr, "value", arccosine)
{}
OperationImplementation *ArccosineCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new ArccosineCoverage(metaid,expr);}

quint64 ArccosineCoverage::createMetadata() {
    Resource resource = UnaryMathRasterAndNumber::populateMetadata(QString("ilwis://operations/acos"), "Arccosine  for rasters");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain with value between -1 and 1; "));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain; values are between 0 and pi"));

    mastercatalog()->addItems({resource});
    return resource.id();
}

//----------------------------------------------------------
REGISTER_OPERATION(ArcTangentCoverage)
ArcTangentCoverage::ArcTangentCoverage(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathRasterAndNumber(metaid, expr, "value", atan)
{}
OperationImplementation *ArcTangentCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new ArcTangentCoverage(metaid,expr);}

quint64 ArcTangentCoverage::createMetadata() {
    Resource resource = UnaryMathRasterAndNumber::populateMetadata(QString("ilwis://operations/atan"), "ArcTangent  for rasters");
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
REGISTER_OPERATION(Log10Coverage)
Log10Coverage::Log10Coverage(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathRasterAndNumber(metaid, expr, "value", logten)
{}
OperationImplementation *Log10Coverage::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new Log10Coverage(metaid,expr);}

quint64 Log10Coverage::createMetadata() {
    Resource resource = UnaryMathRasterAndNumber::populateMetadata(QString("ilwis://operations/log10"), "Log10 for rasters");
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

REGISTER_OPERATION(NaturalLogarithmCoverage)
NaturalLogarithmCoverage::NaturalLogarithmCoverage(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathRasterAndNumber(metaid, expr, "value", naturallog)
{}
OperationImplementation *NaturalLogarithmCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new NaturalLogarithmCoverage(metaid,expr);}

quint64 NaturalLogarithmCoverage::createMetadata() {
    Resource resource = UnaryMathRasterAndNumber::populateMetadata(QString("ilwis://operations/ln"), "Natural Logarithm for rasters");
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
REGISTER_OPERATION(AbsCoverage)
AbsCoverage::AbsCoverage(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathRasterAndNumber(metaid, expr, "value", abs2)
{}
OperationImplementation *AbsCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new AbsCoverage(metaid,expr);}

quint64 AbsCoverage::createMetadata() {
    Resource resource = UnaryMathRasterAndNumber::populateMetadata(QString("ilwis://operations/abs"), "Absolute value  for rasters");
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
REGISTER_OPERATION(SqrtCoverage)
SqrtCoverage::SqrtCoverage(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathRasterAndNumber(metaid, expr, "value", sqrt2)
{}
OperationImplementation *SqrtCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new SqrtCoverage(metaid,expr);}

quint64 SqrtCoverage::createMetadata() {
    Resource resource = UnaryMathRasterAndNumber::populateMetadata(QString("ilwis://operations/sqrt"), "Square root value for rasters");
    resource.addProperty("pin_1_desc",QString("input data source with a positive numerical domain (or 0)"));
    resource.addProperty("pout_1_desc",QString("output data source with a positive numerical domain"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
REGISTER_OPERATION(CeilCoverage)
CeilCoverage::CeilCoverage(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathRasterAndNumber(metaid, expr, "value", std::ceil<int>)
{}
OperationImplementation *CeilCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new CeilCoverage(metaid,expr);}

quint64 CeilCoverage::createMetadata() {
    Resource resource = UnaryMathRasterAndNumber::populateMetadata(QString("ilwis://operations/ceil"), "Ceiling value for rasters");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain"));
    resource.addProperty("pout_1_desc",QString("output data source with a integer numerical domain"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
REGISTER_OPERATION(FloorCoverage)
FloorCoverage::FloorCoverage(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathRasterAndNumber(metaid, expr, "value", std::floor<int>)
{}
OperationImplementation *FloorCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new FloorCoverage(metaid,expr);}

quint64 FloorCoverage::createMetadata() {
    Resource resource = UnaryMathRasterAndNumber::populateMetadata(QString("ilwis://operations/floor"), "Floor value for rasters");
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

REGISTER_OPERATION(SignCoverage)
SignCoverage::SignCoverage(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathRasterAndNumber(metaid, expr,"min1to1", sign)
{}
OperationImplementation *SignCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new FloorCoverage(metaid,expr);}

quint64 SignCoverage::createMetadata() {
    Resource resource = UnaryMathRasterAndNumber::populateMetadata(QString("ilwis://operations/sgn"), "Sign for rasters");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain"));
    resource.addProperty("pout_1_desc",QString("output data source with values -1,0,1 depending on the sign of the input value"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
REGISTER_OPERATION(CosineHCoverage)
CosineHCoverage::CosineHCoverage(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathRasterAndNumber(metaid, expr,"value", cosh)
{}
OperationImplementation *CosineHCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new CosineHCoverage(metaid,expr);}

quint64 CosineHCoverage::createMetadata() {
    Resource resource = UnaryMathRasterAndNumber::populateMetadata(QString("ilwis://operations/cosh"), "Hyperbolic cosine  for rasters");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain; values are considered to be radians"));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
REGISTER_OPERATION(SineHCoverage)
SineHCoverage::SineHCoverage(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathRasterAndNumber(metaid, expr,"value", sinh)
{}
OperationImplementation *SineHCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new SineHCoverage(metaid,expr);}

quint64 SineHCoverage::createMetadata() {
    Resource resource = UnaryMathRasterAndNumber::populateMetadata(QString("ilwis://operations/sinh"), "Hyperbolic sine  for rasters");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain; values are considered to be radians"));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
REGISTER_OPERATION(SineTable)

SineTable::SineTable(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathTable(metaid, expr,"min1to1", sin)
{}
OperationImplementation *SineTable::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new SineTable(metaid,expr);}

quint64 SineTable::createMetadata() {
    Resource resource = UnaryMathTable::populateMetadata(QString("ilwis://operations/sin"), "Sine for Columns");
    resource.addProperty("pin_1_desc", QString("input data source with a numerical domain; values are considered to be radians"));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain; values are between -1 and 1"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
REGISTER_OPERATION(CosineTable)
CosineTable::CosineTable(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathTable(metaid, expr,"min1to1", cos)
{}
OperationImplementation *CosineTable::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new CosineTable(metaid,expr);}

quint64 CosineTable::createMetadata() {
    Resource resource = UnaryMathTable::populateMetadata(QString("ilwis://operations/cos"), "Cosine  for Columns");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain; values are considered to be radians"));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain; values are between -1 and 1"));

    mastercatalog()->addItems({resource});
    return resource.id();
}

//----------------------------------------------------------
REGISTER_OPERATION(TangentTable)
TangentTable::TangentTable(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathTable(metaid, expr, "value",tangent)
{}
OperationImplementation *TangentTable::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new TangentTable(metaid,expr);}

quint64 TangentTable::createMetadata() {
    Resource resource = UnaryMathTable::populateMetadata(QString("ilwis://operations/tan"), "Tangent  for Columns");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain; values are considered to be radians"));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain; "));

    mastercatalog()->addItems({resource});
    return resource.id();
}

//----------------------------------------------------------
REGISTER_OPERATION(ArcsineTable)

ArcsineTable::ArcsineTable(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathTable(metaid, expr, "value", arcsine)
{}
OperationImplementation *ArcsineTable::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new ArcsineTable(metaid,expr);}

quint64 ArcsineTable::createMetadata() {
    Resource resource = UnaryMathTable::populateMetadata(QString("ilwis://operations/asin"), "Arcsine  for Columns");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain;with value between -1 and 1 "));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain; values are between pi/2 and -pi/2"));

    mastercatalog()->addItems({resource});
    return resource.id();
}

//----------------------------------------------------------
REGISTER_OPERATION(ArccosineTable)
ArccosineTable::ArccosineTable(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathTable(metaid, expr, "value", arccosine)
{}
OperationImplementation *ArccosineTable::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new ArccosineTable(metaid,expr);}

quint64 ArccosineTable::createMetadata() {
    Resource resource = UnaryMathTable::populateMetadata(QString("ilwis://operations/acos"), "Arccosine  for Columns");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain with value between -1 and 1; "));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain; values are between 0 and pi"));

    mastercatalog()->addItems({resource});
    return resource.id();
}

//----------------------------------------------------------
REGISTER_OPERATION(ArcTangentTable)
ArcTangentTable::ArcTangentTable(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathTable(metaid, expr, "value", atan)
{}
OperationImplementation *ArcTangentTable::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new ArcTangentTable(metaid,expr);}

quint64 ArcTangentTable::createMetadata() {
    Resource resource = UnaryMathTable::populateMetadata(QString("ilwis://operations/atan"), "ArcTangent  for Columns");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain;with value between -pi/2 and pi/2 "));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain; values are between -1 and 1"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
REGISTER_OPERATION(Log10Table)
Log10Table::Log10Table(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathTable(metaid, expr, "value", logten)
{}
OperationImplementation *Log10Table::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new Log10Table(metaid,expr);}

quint64 Log10Table::createMetadata() {
    Resource resource = UnaryMathTable::populateMetadata(QString("ilwis://operations/log10"), "Log10  for Columns");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain;with value larger than zero "));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
REGISTER_OPERATION(NaturalLogarithmTable)
NaturalLogarithmTable::NaturalLogarithmTable(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathTable(metaid, expr, "value", naturallog)
{}
OperationImplementation *NaturalLogarithmTable::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new NaturalLogarithmTable(metaid,expr);}

quint64 NaturalLogarithmTable::createMetadata() {
    Resource resource = UnaryMathTable::populateMetadata(QString("ilwis://operations/ln"), "Natural Logarithm  for Columns");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain;with value larger than zero "));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------

REGISTER_OPERATION(AbsTable)
AbsTable::AbsTable(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathTable(metaid, expr, "value", abs2)
{}
OperationImplementation *AbsTable::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new AbsTable(metaid,expr);}

quint64 AbsTable::createMetadata() {
    Resource resource = UnaryMathTable::populateMetadata(QString("ilwis://operations/abs"), "Absolute value for Columns");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain"));
    resource.addProperty("pout_1_desc",QString("output data source with a positive numerical domain"));

    mastercatalog()->addItems({resource});
    return resource.id();
}

//---------------------------------------------------------
REGISTER_OPERATION(SqrtTable)
SqrtTable::SqrtTable(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathTable(metaid, expr, "value", sqrt2)
{}
OperationImplementation *SqrtTable::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new SqrtTable(metaid,expr);}

quint64 SqrtTable::createMetadata() {
    Resource resource = UnaryMathTable::populateMetadata(QString("ilwis://operations/sqrt"), "Absolute value  for Columns");
    resource.addProperty("pin_1_desc",QString("input data source with a positive numerical domain (or 0)"));
    resource.addProperty("pout_1_desc",QString("output data source with a positive numerical domain"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
REGISTER_OPERATION(CeilTable)
CeilTable::CeilTable(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathTable(metaid, expr, "value", std::ceil<int>)
{}
OperationImplementation *CeilTable::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new CeilTable(metaid,expr);}

quint64 CeilTable::createMetadata() {
    Resource resource = UnaryMathTable::populateMetadata(QString("ilwis://operations/ceil"), "Ceiling value  for Columns");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain"));
    resource.addProperty("pout_1_desc",QString("output data source with a integer numerical domain"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
REGISTER_OPERATION(FloorTable)
FloorTable::FloorTable(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathTable(metaid, expr, "value", std::floor<int>)
{}
OperationImplementation *FloorTable::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new FloorTable(metaid,expr);}

quint64 FloorTable::createMetadata() {
    Resource resource = UnaryMathTable::populateMetadata(QString("ilwis://operations/floor"), "Floor value  for Columns");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain"));
    resource.addProperty("pout_1_desc",QString("output data source with a integer numerical domain"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
REGISTER_OPERATION(SignTable)
SignTable::SignTable(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathTable(metaid, expr,"min1to1", sign)
{}
OperationImplementation *SignTable::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new FloorTable(metaid,expr);}

quint64 SignTable::createMetadata() {
    Resource resource = UnaryMathTable::populateMetadata(QString("ilwis://operations/sgn"), "Sign  for Columns");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain"));
    resource.addProperty("pout_1_desc",QString("output data source with values -1,0,1 depending on the sign of the input value"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
REGISTER_OPERATION(CosineHTable)
CosineHTable::CosineHTable(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathTable(metaid, expr,"value", cosh)
{}
OperationImplementation *CosineHTable::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new CosineHTable(metaid,expr);}

quint64 CosineHTable::createMetadata() {
    Resource resource = UnaryMathTable::populateMetadata(QString("ilwis://operations/cosh"), "Hyperbolic cosine  for Columns");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain; values are considered to be radians"));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
//----------------------------------------------------------
REGISTER_OPERATION(SineHTable)
SineHTable::SineHTable(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMathTable(metaid, expr,"value", sinh)
{}
OperationImplementation *SineHTable::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new SineHTable(metaid,expr);}

quint64 SineHTable::createMetadata() {
    Resource resource = UnaryMathTable::populateMetadata(QString("ilwis://operations/sinh"), "Hyperbolic sine  for Columns");
    resource.addProperty("pin_1_desc",QString("input data source with a numerical domain; values are considered to be radians"));
    resource.addProperty("pout_1_desc",QString("output data source with a numerical domain"));

    mastercatalog()->addItems({resource});
    return resource.id();
}
