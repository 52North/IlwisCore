#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "numericdomain.h"
#include "symboltable.h"
#include "OperationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "numericoperation.h"

using namespace Ilwis;

NumericOperation::NumericOperation()
{
}

NumericOperation::NumericOperation(quint64 metaid, const Ilwis::OperationExpression &expr) :  OperationImplementation(metaid, expr)
{

}

NumericRange *NumericOperation::constructRangeFrom(const SPNumericRange &range1, const SPNumericRange &range2) const
{
    double rmax, rmin;
    switch(_operator) {
        case otPLUS:
           rmin = range1->min() + range2->min();
           rmax = range1->max() + range2->max();
           break;
        case otMINUS:
            rmin = range1->min() - range2->min();
            rmax = range1->max() - range2->max();
            break;
        case otDIV:
            rmin = range2->min() != 0 ? range1->min() / range2->min() : std::min(range1->min(), range2->min());
            rmax = range2->max() != 0 ? range1->max() / range2->max() : std::min(range1->max(), range2->max());
            break;
        case otMULT:
            rmin = range1->min() * range2->min();
            rmax = range1->max() * range2->max();
            break;
    }
    NumericRange *newRange = new NumericRange(rmin,
                                              rmax,
                                              std::min(range1->resolution(), range2->resolution()));

    return newRange;
}

NumericRange *NumericOperation::constructRangeFrom(const SPNumericRange &range, double number) const
{
    double rmax, rmin;
    switch(_operator) {
        case otPLUS:
           rmin = range->min() + number;
           rmax = range->max() + number;
           break;
        case otMINUS:
            rmin = range->min() - number;
            rmax = range->max() - number;
            break;
        case otDIV:
            rmin = number != 0 ? range->min() / number : range->min();
            rmax = number != 0 ? range->max() / number : range->max();
            break;
        case otMULT:
            rmin = range->min() * number;
            rmax = range->max() * number;
            break;
    }
    NumericRange *newRange = new NumericRange(rmin,
                                              rmax,
                                              range->resolution());

    return newRange;

}

void NumericOperation::mathoperator(const QString &oper)
{
    if ( oper.toLower() == "add")
        _operator = otPLUS;
    else if ( oper.toLower() == "substract")
        _operator = otMINUS;
    else if ( oper == "divide")
        _operator = otDIV;
    else
        _operator = otMULT;
}
