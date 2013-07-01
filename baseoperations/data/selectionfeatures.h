#ifndef SELECTIONFEATURES_H
#define SELECTIONFEATURES_H

namespace Ilwis {
namespace BaseOperations {

class SelectionFeatures : public OperationImplementation
{
public:
    SelectionFeatures();
    SelectionFeatures(quint64 metaid, const Ilwis::OperationExpression &expr);
    ~SelectionFeatures();

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable&);

    static quint64 createMetadata();
private:
    IIlwisObject _inputObj;
    IIlwisObject _outputObj;
    QString _attribColumn;
    ITable _attTable;
    Box3D<double> _box;
};
}
}

#endif // SELECTIONFEATURES_H
