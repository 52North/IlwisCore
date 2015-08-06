#ifndef STRINGCOLUMN2ITEMCOLUMN_H
#define STRINGCOLUMN2ITEMCOLUMN_H

namespace Ilwis {
namespace BaseOperations {

class ConvertColumnDomain :  public OperationImplementation
{
public:
    ConvertColumnDomain();

    ConvertColumnDomain(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(ConvertColumnDomain);

private:
    ITable _inputTable;
    QString _columnName;
    QString _targetDomainType;
    IlwisTypes _sourceDomainType;
    QString _domainName;
};
}
}

#endif // STRINGCOLUMN2ITEMCOLUMN_H
