#ifndef UNARYMATH_H
#define UNARYMATH_H

namespace Ilwis {
namespace BaseOperations{

typedef std::function<double(double)> UnaryFunction;

class UnaryMath : public OperationImplementation
{
public:
    enum OperationType{otSPATIAL, otTABLE, otNUMBER};
    enum UnaryOperations{uoSIN, uoCOS, uoTAN, uoSQRT, uoASIN, uoACOS, uoATAN, uoLog10, uoLN, uoABS, uoCEIL,
                         uoFLOOR,uoCOSH, uoEXP, uoNEG,uoRND,uoSGN,uoSINH,uoTANH};
    UnaryMath();
    UnaryMath(quint64 metaid, const Ilwis::OperationExpression &expr, const QString& outputdom, UnaryFunction fun);

protected:
    OperationType _case;
    UnaryFunction _unaryFun;
    QString _outputDomain;

};
}
}

#endif // UNARYMATH_H
