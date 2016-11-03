#include <functional>
#include <future>
#include "kernel.h"
#include "coverage.h"
#include "geos/geom/Point.h"
#include "geos/geom/GeometryFactory.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"
#include "table.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "operationhelperfeatures.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "geometryhelper.h"
#include "geos/geom/LineString.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "polygon2line.h"
#include "line2polygon.h"

using namespace Ilwis;
using namespace FeatureOperations;

REGISTER_OPERATION(Line2Polygon)

Line2Polygon::Line2Polygon()
{

}

Line2Polygon::Line2Polygon(quint64 metaid, const Ilwis::OperationExpression &expr)
{

}

bool Line2Polygon::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    return false;
}

Ilwis::OperationImplementation *Line2Polygon::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Line2Polygon(metaid, expr)    ;
}

Ilwis::OperationImplementation::State Line2Polygon::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    return sPREPARED;
}

quint64 Line2Polygon::createMetadata()
{
    return i64UNDEF;
}
