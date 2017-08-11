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

protected:
    IDomain _domain;
    IDomain _stackDomain;
    IGeoReference _grf;
    std::vector<QString> _stackValueStrings;
    std::vector<double> _stackValueNumbers;
    std::vector<Range *> _ranges;
    std::vector<IRasterCoverage> _bands;
    bool _autoresample = false;
    IRasterCoverage _outputRaster;
    bool _empty = false;

    bool parseStackDefintion(const QString &stackDef);
    bool parseStackDefintionNumericCase(const QString &stackDef);
    bool parseStackDefintionTimeCase(const QString &stackDef);
    QString expandWildCards(const QString &wildmaps);
};

class CreateSimpelRasterCoverage :  public CreateRasterCoverage
{
public:
    CreateSimpelRasterCoverage();

    CreateSimpelRasterCoverage(quint64 metaid, const Ilwis::OperationExpression &expr);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(CreateSimpelRasterCoverage);
    bool execute(ExecutionContext *ctx, SymbolTable &symTable);
};
}
}
#endif // CREATERASTERCOVERAGE_H
