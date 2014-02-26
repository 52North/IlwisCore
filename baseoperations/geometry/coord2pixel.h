#ifndef Coord2Pixel_H
#define Coord2Pixel_H

namespace Ilwis {
class Coord2Pixel : public OperationImplementation
{
public:
    Coord2Pixel();
    Coord2Pixel(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
protected:
    Ilwis::OperationImplementation::State prepare(Ilwis::ExecutionContext *ctx, const Ilwis::SymbolTable &symTable);
    IRasterCoverage _inputGC;
    Coordinate _coord;
    QString _outName=sUNDEF;

    NEW_OPERATION(Coord2Pixel);
};
}

#endif // Coord2Pixel_H
