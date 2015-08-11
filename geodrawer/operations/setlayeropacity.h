#ifndef SETLAYERTRANSPARENCY_H
#define SETLAYERTRANSPARENCY_H

namespace Ilwis {
namespace Geodrawer {

class DrawerInterface;

class SetLayerOpacity : public DrawerOperation
{
public:
    SetLayerOpacity();
    ~SetLayerOpacity();

    SetLayerOpacity(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();
private:
    NEW_OPERATION(SetLayerOpacity);
    DrawerInterface *_drawer;
    float _alpha = 1.0;
};
}
}

#endif // SETLAYERTRANSPARENCY_H
