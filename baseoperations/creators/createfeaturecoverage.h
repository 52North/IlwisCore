#ifndef CREATEFEATURECOVERAGE_H
#define CREATEFEATURECOVERAGE_H

namespace Ilwis {
namespace BaseOperations {
class CreateFeatureCoverage :  public OperationImplementation
{
public:
    CreateFeatureCoverage();

    CreateFeatureCoverage(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(CreateFeatureCoverage);

private:
    IDomain _stackDomain;
    ICoordinateSystem _csy;
    std::map<QString, IFeatureCoverage> _features;
    std::map<int, std::vector<ColumnDefinition>> _attributeDefinitions;


    void registerColumnDefinitions(const IFeatureCoverage &fc, int stackIndex);
};
}
}

#endif // CREATEFEATURECOVERAGE_H
