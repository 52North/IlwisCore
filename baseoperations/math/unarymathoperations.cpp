#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "ilwisoperation.h"
#include "pixeliterator.h"
#include "unarymath.h"
#include "unarymathoperations.h"

using namespace Ilwis;
using namespace BaseOperations;

Sinus::Sinus(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMath(metaid, expr, "min1to1", sin)
{}
OperationImplementation *Sinus::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new Sinus(metaid,expr);}

quint64 Sinus::createMetadata() {
    Resource res = UnaryMath::populateMetadata(QString("ilwis://operations/sin"), "sine", "min1to1");
    res.addProperty("pin_1_desc","input gridcoverage with a numerical domain; values are considered to be radians");
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_desc","output gridcoverage with a numerical domain; values are between -1 and 1");

    mastercatalog()->addItems({res});
    return res.id();
}
//----------------------------------------------------------
Cosine::Cosine(quint64 metaid,const Ilwis::OperationExpression& expr) : UnaryMath(metaid, expr, "min1to1", sin)
{}
OperationImplementation *Cosine::create(quint64 metaid, const Ilwis::OperationExpression &expr){return new Cosine(metaid,expr);}

quint64 Cosine::createMetadata() {
    Resource res = UnaryMath::populateMetadata(QString("ilwis://operations/cos"), "cosine", "min1to1");
    res.addProperty("pin_1_desc","input gridcoverage with a numerical domain; values are considered to be radians");
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_desc","output gridcoverage with a numerical domain; values are between -1 and 1");

    mastercatalog()->addItems({res});
    return res.id();
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
    Resource res = UnaryMath::populateMetadata(QString("ilwis://operations/tan"), "Tangent", "value");
    res.addProperty("pin_1_desc","input gridcoverage with a numerical domain; values are considered to be radians");
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_desc","output gridcoverage with a numerical domain; ");

    mastercatalog()->addItems({res});
    return res.id();
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
    Resource res = UnaryMath::populateMetadata(QString("ilwis://operations/asin"), "Arcsine", "value");
    res.addProperty("pin_1_desc","input gridcoverage with a numerical domain;with value between -1 and 1 ");
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_desc","output gridcoverage with a numerical domain; values are between pi/2 and -pi/2");

    mastercatalog()->addItems({res});
    return res.id();
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
    Resource res = UnaryMath::populateMetadata(QString("ilwis://operations/acos"), "Arccosine", "value");
    res.addProperty("pin_1_desc","input gridcoverage with a numerical domain with value between -1 and 1; ");
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_desc","output gridcoverage with a numerical domain; values are between 0 and pi");

    mastercatalog()->addItems({res});
    return res.id();
}

//----------------------------------------------------------
