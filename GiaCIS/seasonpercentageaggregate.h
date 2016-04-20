#ifndef SEASONPERCENTAGEAGGREGATE_H
#define SEASONPERCENTAGEAGGREGATE_H

namespace Ilwis {
namespace GiaCIS {

class SeasonPercentageAggregate : public OperationImplementation
{
public:
    SeasonPercentageAggregate();
    SeasonPercentageAggregate(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    const int _PSERIES = 5;

private:
    double percentage(std::pair<double, double> lims, double val);

    IIlwisObject _inputObj;
    IIlwisObject _inputZones;
    IIlwisObject _lowPercentile;
    IIlwisObject _highPercentile;
    IIlwisObject _seasonTable;
    IIlwisObject _outputObj;

    // parameters
    int _nb;                       // number of maps in maplist
    int _nptperyear = 36;          // number of maps per year
    int _startDekad;               // start date as day of year
    bool _doRunning;

    NEW_OPERATION(SeasonPercentageAggregate);
};
}
}
#endif // SEASONPERCENTAGEAGGREGATE_H
