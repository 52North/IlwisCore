#ifndef CompareHistograms_H
#define CompareHistograms_H

namespace Ilwis {
namespace OpenCV {


class CompareHistograms : public OpenCVOperation
{
public:
    CompareHistograms();


    CompareHistograms(quint64 metaid, const Ilwis::OperationExpression &expr);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(CompareHistograms);

private:

    IRasterCoverage _inputRaster1;
    IRasterCoverage _inputRaster2;
    bool _accumulate = false;
    int _marker;


    bool execute(ExecutionContext *ctx, SymbolTable &symTable);
};
}
}

#endif // CompareHistograms_H
