#ifndef NUMERICOPERATION_H
#define NUMERICOPERATION_H

namespace Ilwis {
class KERNELSHARED_EXPORT NumericOperation : public OperationImplementation
{
public:
    enum OperatorType{ otPLUS, otMINUS, otMULT, otDIV};

    NumericOperation();



protected:
    NumericOperation(quint64 metaid,const Ilwis::OperationExpression &expr);

    NumericRange *constructRangeFrom(const SPNumericRange& range1, const SPNumericRange& range2) const;
    NumericRange *constructRangeFrom(const SPNumericRange& range, double number) const;
    void mathoperator(const QString& oper);
    double calc(double v1, double v2) const{
        if ( v1 != rUNDEF && v2 != rUNDEF) {
            switch(_operator) {
            case otPLUS:
                return v1 + v2;
            case otMINUS:
                return v1 - v2;
            case otDIV:
                if ( v2 != 0)
                    return v1 / v2;
                else
                    return rUNDEF;
                break;
            case otMULT:
                return v1 * v2;break;
            }
        }
        return rUNDEF;
    }

    double _number1 = rUNDEF;
    double _number2 = rUNDEF;
    OperatorType _operator;
};
}

#endif // NUMERICOPERATION_H
