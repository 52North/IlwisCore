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
    UnaryMath(quint64 metaid, const Ilwis::OperationExpression &expr, const QString& outpDom, UnaryFunction fun);

protected:
    static Resource populateMetadata(const QString &item, const QString &longname, const QString& outputDom);
    Ilwis::OperationImplementation::State prepare(Ilwis::ExecutionContext *, const Ilwis::SymbolTable &);
    bool execute( ExecutionContext *ctx, SymbolTable& symTable);

    IRasterCoverage _inputGC;
    IRasterCoverage _outputGC;
    ITable _inputTable;
    ITable _outputTable;
    QString _outColumn;
    QString _inColumn;
    Box3D<qint32> _box;
    OperationType _case;
    double _number;
    QString _outputDomain;
    UnaryFunction _unaryFun;

};
}
}

#endif // UNARYMATH_H
