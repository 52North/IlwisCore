#ifndef PIXEL2COORD_H
#define PIXEL2COORD_H

namespace Ilwis {
class Pixel2Coord : public OperationImplementation
{
public:
    Pixel2Coord();
    Pixel2Coord(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
protected:
    Ilwis::OperationImplementation::State prepare(Ilwis::ExecutionContext *ctx, const Ilwis::SymbolTable &symTable);
    IRasterCoverage _inputGC;
    Pixel _pixel;
    QString _outName=sUNDEF;

    NEW_OPERATION(Pixel2Coord);
};
}

#endif // PIXEL2COORD_H
