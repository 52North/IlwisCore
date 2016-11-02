#ifndef ADDDRAWER_H
#define ADDDRAWER_H

#include "iooptions.h"

namespace Ilwis {
namespace Geodrawer {

class AddDrawer : public DrawerOperation
{
public:
    AddDrawer();
    ~AddDrawer();

    AddDrawer(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
    ICoverage _coverage;
    QString _drawerCode;
    IOOptions _options;

    NEW_OPERATION(AddDrawer);
};
}
}

#endif // ADDDRAWER_H
