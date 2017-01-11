#ifndef STACKMINMAXFILTER_H
#define STACKMINMAXFILTER_H

namespace Ilwis {
namespace RasterOperations {


class StackMinMaxFilter : public OperationImplementation
{
    StackMinMaxFilter();

    StackMinMaxFilter(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
    enum StackFilters{sfMAX, sfMIN};

    IRasterCoverage _inputRaster;
    IRasterCoverage _outputRaster;
    StackFilters _filterType;

    NEW_OPERATION(StackMinMaxFilter);
};
}
}

#endif // STACKMINMAXFILTER_H
