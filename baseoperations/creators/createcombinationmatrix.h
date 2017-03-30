#ifndef CREATECOMBINATIONMATRIX_H
#define CREATECOMBINATIONMATRIX_H


namespace Ilwis {
namespace BaseOperations {

class CreateCombinationMatrix :  public OperationImplementation
{
public:
    CreateCombinationMatrix();

    CreateCombinationMatrix(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(CreateCombinationMatrix);

private:
    IDomain _xaxisDomain;
    IDomain _yaxisDomain;
    IDomain _comboDomain;
    QStringList _xAxisValues;
    QStringList _yAxisValues;
    std::vector<double> _values;
};
}
}
#endif // CREATECOMBINATIONMATRIX_H
