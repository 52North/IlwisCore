#ifndef CREATENUMERICDOMAIN_H
#define CREATENUMERICDOMAIN_H

namespace Ilwis {
namespace BaseOperations {

class CreateNumericDomain :  public OperationImplementation
{
public:
    CreateNumericDomain();

    CreateNumericDomain(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(CreateNumericDomain);

private:
    double _vmin, _vmax, _vres;
    IDomain _parent;
    bool _strict;
    QString _domainname, _domaindesc;
};
}
}

#endif // CREATENUMERICDOMAIN_H
