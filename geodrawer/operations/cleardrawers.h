#ifndef CLEARDRAWERS_H
#define CLEARDRAWERS_H


namespace Ilwis {
namespace Geodrawer {

class ClearDrawers : public DrawerOperation
{
public:
    ClearDrawers();
    ~ClearDrawers();

    ClearDrawers(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
    ICoverage _coverage;
    QString _drawerCode;

    NEW_OPERATION(ClearDrawers);
};
}
}

#endif // CLEARDRAWERS_H
