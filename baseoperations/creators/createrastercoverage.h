#ifndef CREATERASTERCOVERAGE_H
#define CREATERASTERCOVERAGE_H


namespace Ilwis {
namespace BaseOperations {
class CreateRasterCoverage :  public OperationImplementation
{
public:
    CreateRasterCoverage();

    CreateRasterCoverage(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(CreateRasterCoverage);

private:
    IDomain _domain;
    IGeoReference _grf;
    QString _desc;
    QString _keywords;
    int _bands;

};
}
}
#endif // CREATERASTERCOVERAGE_H
