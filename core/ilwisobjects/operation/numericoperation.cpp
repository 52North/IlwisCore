#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "numericdomain.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
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
            rmin = range1->min() - range2->max();
            rmax = range1->max() - range2->min();
            break;
        case otDIV:
            if (range2->max() < 0 || range2->min() > 0) {
                rmin = min(range1->min() / range2->min(), range1->max() / range2->min()); // this depends on the sign of both ranges; alternatively one could write out all possible combinations of signs, but using min() and max() is less work
                rmin = min(rmin, range1->min() / range2->max());
                rmin = min(rmin, range1->max() / range2->max());
                rmax = max(range1->min() / range2->min(), range1->max() / range2->min());
                rmax = max(rmax, range1->min() / range2->max());
                rmax = max(rmax, range1->max() / range2->max());
            } else { // both positive and negative infinity are possible outcomes
                rmin = -1e100;
                rmax = 1e100;
            }
            rmin = range2->min() != 0 ? range1->min() / range2->min() : std::min(range1->min(), range2->min());
            rmax = range2->max() != 0 ? range1->max() / range2->max() : std::min(range1->max(), range2->max());
            break;
        case otMULT:
            rmin = min(range1->min() * range2->min(), range1->max() * range2->min()); // this depends on the sign of both ranges; alternatively one could write out all possible combinations of signs, but using min() and max() is less work
            rmin = min(rmin, range1->min() * range2->max());
            rmin = min(rmin, range1->max() * range2->max());
            rmax = max(range1->min() * range2->min(), range1->max() * range2->min());
            rmax = max(rmax, range1->min() * range2->max());
            rmax = max(rmax, range1->max() * range2->max());
            break;
        case otPOW:
            rmin = -1e100;
            rmax = 1e100;
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
            rmin = number != 0 ? min(range->min() / number, range->max() / number) : range->min(); // this is non-trivial: it depends on the sign of number and the sign of the range, and on whether number is between 0 and 1 or greater than 1, and on the range width
            rmax = number != 0 ? max(range->min() / number, range->max() / number) : range->max();
            break;
        case otMULT:
            rmin = number >= 0 ? range->min() * number : range->max() * number;
            rmax = number >= 0 ? range->max() * number : range->min() * number;
            break;
        case otPOW:
            rmin = -1e100;
            rmax = 1e100;
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
    else if ( oper.toLower() == "times")
        _operator = otMULT;
    else if ( oper.toLower() == "power")
        _operator = otPOW;
}
