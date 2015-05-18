#ifndef TIMESAT_NRS_H
#define TIMESAT_NRS_H

namespace Ilwis {
namespace BaseOperations {
class Timesat : public OperationImplementation
{
public:
    Timesat();
    Timesat(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
    IIlwisObject _inputObj;
    IIlwisObject _outputObj;

    // parameters
    int _nb;                    // number of maps in maplist
    std::vector<int> _win;      // array of window sizes for fitting
    int _nptperyear = 36;       // number of maps per year
    double _spikecutoff = 0.5;  // cutoff for spike detection
    bool _forceUpperEnvelope;
    bool _lastIterationLikeTIMESATfit;
    bool _extendWindow;

    bool calcFitWindow(const int i, const int ienvi,
                       const std::vector<double> yfit, const std::vector<bool> wfit,
                       double win_thresh,
                       int org_offset,
                       int& m1, int& m2);
    std::vector<bool> detectSpikes(const std::vector<double> y, std::vector<bool> valid);
    std::vector<double> savgol(std::vector<double> y, std::vector<bool> w);

    NEW_OPERATION(Timesat);
};
}
}

#endif // TIMESAT_NRS_H
