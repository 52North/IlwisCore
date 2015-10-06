#ifndef MOVINGAVERAGE_H
#define MOVINGAVERAGE_H


#define EPS10 1.0e-10
#define EPS20 1.0e-20

namespace Ilwis {
namespace RasterOperations {
class MovingAverage : public OperationImplementation
{
public:
    enum WeightFuncType {wfEXACT = 0, wfNOTEXACT };

    MovingAverage();
    ~MovingAverage();
    MovingAverage(quint64 metaid, const Ilwis::OperationExpression &expr);
    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);
    static quint64 createMetadata();
    double rInvDist(double rDis);
    double rLinDecr(double rDis);

    NEW_OPERATION(MovingAverage);


private:
    IFeatureCoverage _inputfeatures;
    IRasterCoverage _outputraster;
    IGeoReference _inputgrf;
    QString weightFunct;
    double exp;
    double limDist;
    bool _needCoordinateTransformation = false;    
    long iNrPoints;
    long iNrValidPnts;
    WeightFuncType wft;




};
}
}

#endif // MOVINGAVERAGE_H
