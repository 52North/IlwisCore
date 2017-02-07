#ifndef RELATIVEDEM_H
#define RELATIVEDEM_H

namespace Ilwis {
namespace Hydroflow {

class RelativeDEM : public OperationImplementation
{
public:
    RelativeDEM();
    RelativeDEM(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
    IIlwisObject _inputDEM;
    IIlwisObject _inputFlowDir;
    IIlwisObject _inputIsDrain;
    IIlwisObject _outputObj;
    long _xsize, _ysize;

    Pixel nextPosition(const Pixel& start, int dir);
    std::vector<Pixel> findStream(const PixelIterator& iterFlow, const PixelIterator& iterIsDrain);
    bool onEdge(Pixel pix);

    // parameters

    NEW_OPERATION(RelativeDEM);
};
}
}

#endif // RELATIVEDEM_H
