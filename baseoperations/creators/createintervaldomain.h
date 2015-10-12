#ifndef CREATEINTERVALDOMAIN_H
#define CREATEINTERVALDOMAIN_H


namespace Ilwis {
namespace BaseOperations {
class CreateIntervalDomain :  public OperationImplementation
{
public:
    CreateIntervalDomain();

    CreateIntervalDomain(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(CreateIntervalDomain);

private:
    IDomain _parentdomain;
    IDomain _targetdomain;
    QStringList _items;
    bool _strict;
    QString _domaindesc;
    double _resolution;
};
}
}

#endif // CREATEINTERVALDOMAIN_H
