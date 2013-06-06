#ifndef UNARYMATH_H
#define UNARYMATH_H

namespace Ilwis {
namespace BaseOperations{

typedef std::function<double(double)> UnaryFunction;

class UnaryMath : public OperationImplementation
{
public:
    enum UnaryOperations{uoSIN, uoCOS, uoTAN, uoSQRT, uoASIN, uoACOS, uoATAN, uoLog10, uoLN, uoABS, uoCEIL,
                         uoFLOOR,uoCOSH, uoEXP, uoNEG,uoRND,uoSGN,uoSINH,uoTANH};
    UnaryMath();
    UnaryMath(quint64 metaid, const Ilwis::OperationExpression &expr, const QString& outpDom, UnaryFunction fun);

protected:
    static Resource populateMetadata(const QString &item, const QString &longname, const QString& outputDom);
    Ilwis::OperationImplementation::State prepare();
    bool execute( ExecutionContext *ctx);

    IGridCoverage _inputGC;
    IGridCoverage _outputGC;
    Box3D<qint32> _box;
    bool _spatialCase;
    double _number;
    UnaryFunction _unaryFun;
    QString _outputDomain;

};
}
}

#endif // UNARYMATH_H
