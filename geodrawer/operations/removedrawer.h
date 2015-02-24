#ifndef REMOVEDRAWER_H
#define REMOVEDRAWER_H

namespace Ilwis {
namespace Geodrawer {

class RemoveDrawer : public DrawerOperation
{
public:
    RemoveDrawer();
    ~RemoveDrawer();

    RemoveDrawer(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();
private:
   NEW_OPERATION(RemoveDrawer);
   QString _drawercode = sUNDEF;
   qint32 _drawerOrder = iUNDEF;
   DrawerInterface::DrawerType _drawerType;
};
}
}

#endif // REMOVEDRAWER_H
