#ifndef WORKINGCATALOG_H
#define WORKINGCATALOG_H

namespace Ilwis {
namespace BaseOperations {
class WorkingCatalog : public OperationImplementation
{
public:
    WorkingCatalog();

    WorkingCatalog(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *, const Ilwis::SymbolTable &symTable);

    static quint64 createMetadata();
private:
    QUrl _location;

    NEW_OPERATION(WorkingCatalog)    ;



};
}
}

#endif // WORKINGCATALOG_H
