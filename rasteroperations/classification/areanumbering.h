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

    NEW_OPERATION(AreaNumbering);

};

class AreaNumberer {
public:
    AreaNumberer(quint32 xsize, quint8 connectivity);
    std::map<int,int> pass2Entries();
    quint32 value(const Ilwis::PixelIterator &inIter);

    quint32 lastid() const;

private:
    quint8 _connectivity;
    quint32 _currentId = 0;
    std::vector<double> _neighboursIn;
    std::vector<double> _neighboursOut;
    std::map<int,int> _pass2Changes;
    double do4connected(const Ilwis::PixelIterator &in);

};
}
}

#endif // AreaNumbering_H
