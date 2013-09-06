#ifndef AreaNumbering_H
#define AreaNumbering_H

namespace Ilwis {
namespace RasterOperations {
class AreaNumbering : public OperationImplementation
{
public:
    AreaNumbering();
    AreaNumbering(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();


private:
    IIlwisObject _inputObj;
    IIlwisObject _outputObj;
    quint8 _connectivity;

};

class AreaNumberer {
public:
    AreaNumberer(quint32 xsize, quint8 connectivity);

    quint32 value(const Ilwis::PixelIterator &inIter);

private:
    quint8 _connectivity;
    quint32 _currentId = 0;
    std::vector<double> _neighboursIn;
    std::vector<double> _neighboursOut;
    double do4connected(const Ilwis::PixelIterator &in);
};
}
}

#endif // AreaNumbering_H
