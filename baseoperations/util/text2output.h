#ifndef TEXT2OUTPUT_H
#define TEXT2OUTPUT_H

namespace Ilwis {
namespace BaseOperations {
class Text2Output : public OperationImplementation
{
public:
    Text2Output();

    Text2Output(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *, const Ilwis::SymbolTable &symTable);

    static quint64 createMetadata();

private:
    QString _text;

    NEW_OPERATION(Text2Output)    ;
};
}
}

#endif // TEXT2OUTPUT_H
