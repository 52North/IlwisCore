#ifndef PERCENTILEFILTERSTRETCH_H
#define PERCENTILEFILTERSTRETCH_H

namespace Ilwis {
namespace GiaCIS {

class PercentileFilterStretch : public OperationImplementation
{
public:
    PercentileFilterStretch();
    PercentileFilterStretch(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
    IIlwisObject _inputObj;
    IIlwisObject _inputZones;
    IIlwisObject _lowPercentile;
    IIlwisObject _highPercentile;
    IIlwisObject _outputObj;

    // parameters
    int _nb;                       // number of maps in maplist
    int _nptperyear = 36;          // number of maps per year
    int _startDekad;               // start date as day of year

    NEW_OPERATION(PercentileFilterStretch);
};
}
}

#endif // PERCENTILEFILTERSTRETCH_H
