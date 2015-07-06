#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "columndefinition.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "unarymath.h"

using namespace Ilwis;
using namespace BaseOperations;

UnaryMath::UnaryMath() {

}

UnaryMath::UnaryMath(quint64 metaid, const Ilwis::OperationExpression& expr, const QString &outputdom, UnaryFunction fun) :
    OperationImplementation(metaid, expr),
    _case(otSPATIAL),
    _unaryFun(fun),
    _outputDomain(outputdom)
{

}




