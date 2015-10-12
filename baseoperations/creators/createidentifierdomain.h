#ifndef CREATEIDENTIFIERDOMAIN_H
#define CREATEIDENTIFIERDOMAIN_H


namespace Ilwis {
namespace BaseOperations {
class CreateIdentifierDomain :  public OperationImplementation
{
public:
    CreateIdentifierDomain();

    CreateIdentifierDomain(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(CreateIdentifierDomain);

private:
    IDomain _parentdomain;
    IDomain _targetdomain;
    QStringList _items;
    bool _strict;
    QString _domaindesc;
};
}
}
#endif // CREATEIDENTIFIERDOMAIN_H
