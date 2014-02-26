#ifndef MASTERGEOREFERENCE_H
#define MASTERGEOREFERENCE_H

namespace Ilwis {
namespace BaseOperations {

class MasterGeoreference : public OperationImplementation
{
public:
    MasterGeoreference();
    MasterGeoreference(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();
private:
    IGeoReference _georef;

    NEW_OPERATION(MasterGeoreference);
};
}
}

#endif // MASTERGEOREFERENCE_H
