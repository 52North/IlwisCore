#ifndef SETLAYERVISIBILITY_H
#define SETLAYERVISIBILITY_H

namespace Ilwis {
namespace Geodrawer {


class SetLayerVisibility : public DrawerOperation
{
public:
    SetLayerVisibility();
    ~SetLayerVisibility();

    SetLayerVisibility(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(SetLayerVisibility);

private:
    int _index = iUNDEF;
    QString _code = sUNDEF;
    bool _visibility;
    DrawerInterface::DrawerType _type = DrawerInterface::dtDONTCARE;
};
}
}

#endif // SETLAYERVISIBILITY_H
