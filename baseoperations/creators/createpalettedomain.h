#ifndef CREATEPALETTEDOMAIN_H
#define CREATEPALETTEDOMAIN_H


namespace Ilwis {
namespace BaseOperations {
class CreatePaletteDomain :  public OperationImplementation
{
public:
    CreatePaletteDomain();

    CreatePaletteDomain(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(CreatePaletteDomain);

private:
    IDomain _parentdomain;
    IDomain _targetdomain;
    QStringList _items;
    bool _strict;
    QString _domaindesc;
};
}
}
#endif // CREATEPALETTEDOMAIN_H
