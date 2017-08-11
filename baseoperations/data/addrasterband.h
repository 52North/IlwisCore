#ifndef ADDRASTERBAND_H
#define ADDRASTERBAND_H


namespace Ilwis {
namespace BaseOperations{
class AddRasterBand  : public OperationImplementation
{
public:
    AddRasterBand();
    AddRasterBand(quint64 metaid, const Ilwis::OperationExpression &expr);
    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    State prepare(ExecutionContext *ctx,const SymbolTable&);

private:
    IRasterCoverage _inputRaster;
    IRasterCoverage _band;

    NEW_OPERATION(AddRasterBand)    ;
};
}
}
#endif // ADDRASTERBAND_H
