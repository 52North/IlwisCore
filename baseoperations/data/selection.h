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

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable&);

    static quint64 createMetadata();
private:
    IIlwisObject _inputObj;
    IIlwisObject _outputObj;
    QString _attribColumn;
    Box3D<qint32> _box;
    std::vector<qint32> _base;
    int _zvalue = iUNDEF;


};
}
}
#endif // SELECTION_H
