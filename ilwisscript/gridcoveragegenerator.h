#ifndef GRIDCOVERAGEGENERATOR_H
#define GRIDCOVERAGEGENERATOR_H

namespace Ilwis {

class GridCoverageGenerator;

typedef double (*DFunctionD)(double v);

struct Slice {
    Slice() : _status(false), _minLine(0), _maxline(0), _outputgc(0) {}
    DFunctionD _calc;
    unsigned long _minLine;
    unsigned long _maxline;
    IGridCoverage _inputgc;
    GridCoverageGenerator *_outputgc;
    bool _status;

    void calculateSlice();

};


class GridCoverageGenerator : public GridCoverage
{
public:
    GridCoverageGenerator();
    bool generateFrom(const IGridCoverage &inputGC, DFunctionD func);
    bool setSize(const Box2D<qint32> &box, const Box2D<double> bounds);
    bool setDomain(const QString& name);
};

typedef IlwisData<GridCoverageGenerator> IGridCoverageGenerator;
}

#endif // GRIDCOVERAGEGENERATOR_H
