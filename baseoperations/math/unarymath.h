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
    Ilwis::OperationImplementation::State prepare(Ilwis::ExecutionContext *, const Ilwis::SymbolTable &);
    bool execute( ExecutionContext *ctx, SymbolTable& symTable);

    IRasterCoverage _inputGC;
    IRasterCoverage _outputGC;
    Box3D<qint32> _box;
    bool _spatialCase;
    double _number;
    QString _outputDomain;
    UnaryFunction _unaryFun;

};
}
}

#endif // UNARYMATH_H
