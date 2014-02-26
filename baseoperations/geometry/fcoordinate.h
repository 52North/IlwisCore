#ifndef FCOORDINATE_H
#define FCOORDINATE_H

namespace Ilwis {
namespace BaseOperations {
class FCoordinate : public OperationImplementation
{
public:
    FCoordinate();

    FCoordinate(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
private:
    Coordinate _coord;

    NEW_OPERATION(FCoordinate);
};
}
}

#endif // FCOORDINATE_H
