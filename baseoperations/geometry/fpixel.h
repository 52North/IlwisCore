#ifndef FPixel_H
#define FPixel_H

namespace Ilwis {
namespace BaseOperations {
class FPixel : public OperationImplementation
{
public:
    FPixel();

    FPixel(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable&);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
private:
    Pixel _pixel;

    NEW_OPERATION(FPixel);
};
}
}

#endif // FPixel_H
