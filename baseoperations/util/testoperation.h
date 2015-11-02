#ifndef TESTOPERATION_H
#define TESTOPERATION_H


namespace Ilwis {
namespace BaseOperations {

class TestOperation :  public OperationImplementation
{
public:
    TestOperation();

    TestOperation(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(TestOperation);

private:
    IRasterCoverage _raster1;
    IRasterCoverage _raster2;
};
}
}

#endif // TESTOPERATION_H
