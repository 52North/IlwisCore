#ifndef SETLAYERTRANSPARENCY_H
#define SETLAYERTRANSPARENCY_H

namespace Ilwis {
namespace Geodrawer {

class DrawerInterface;

class SetLayerTransparency : public DrawerOperation
{
public:
    SetLayerTransparency();
    ~SetLayerTransparency();

    SetLayerTransparency(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();
private:
    NEW_OPERATION(SetLayerTransparency);
    DrawerInterface *_drawer;
    float _alpha;
};
}
}

#endif // SETLAYERTRANSPARENCY_H
