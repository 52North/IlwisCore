#ifndef SETBACKGROUNDCOLOR_H
#define SETBACKGROUNDCOLOR_H

namespace Ilwis {
namespace Geodrawer {

class SetBackgroundColor : public DrawerOperation
{
public:
    SetBackgroundColor();

    SetBackgroundColor(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(SetBackgroundColor);

private:
    QColor _backgroundColor;
};
}
}

#endif // SETBACKGROUNDCOLOR_H
