#ifndef PostgresqlConnection_H
#define PostgresqlConnection_H

#include "postgresqlparameters.h"

namespace Ilwis {
namespace Postgresql {

class PostgresqlConnection :public OperationImplementation
{

public:
    PostgresqlConnection(quint64 metaid, const Ilwis::OperationExpression &expr);
    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);
    static quint64 createMetadata();

    NEW_OPERATION(PostgresqlConnection);

private:
    PostgresqlParameters _params;
};
}
}

#endif // PostgresqlConnection_H
