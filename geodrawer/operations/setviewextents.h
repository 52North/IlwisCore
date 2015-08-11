#ifndef SETVIEWBOUNDARY_H
#define SETVIEWBOUNDARY_H

namespace Ilwis {
namespace Geodrawer {

class SetViewExtent : public DrawerOperation
{
public:
    SetViewExtent();
    ~SetViewExtent();

    SetViewExtent(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
   NEW_OPERATION(SetViewExtent);
   Envelope _newExtents;
   bool _entiremap = false;
};
}
}

#endif // SETVIEWBOUNDARY_H
