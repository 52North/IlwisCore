#ifndef SELECTION_H
#define SELECTION_H


namespace Ilwis {
namespace BaseOperations {

class Selection : public OperationImplementation
{
public:
    Selection();
    Selection(quint64 metaid, const Ilwis::OperationExpression &expr);
    ~Selection();

    bool execute(ExecutionContext *ctx=0);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare();

    static quint64 createMetadata();
private:
    IGridCoverage _inputGC;
    IGridCoverage _outputGC;
    QString _attribColumn;
    Box3D<qint32> _box;
    std::vector<qint32> _base;
};
}
}
#endif // SELECTION_H
