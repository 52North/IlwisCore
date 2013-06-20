#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

namespace Ilwis {
namespace BaseOperations {

class Assignment : public OperationImplementation
{
public:
    Assignment();
    Assignment(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable&);

    static quint64 createMetadata();
private:
    template<typename T> void setOutput(ExecutionContext *ctx,SymbolTable& symTable) {
        QVariant value;
        IlwisData<T> outputObj = _outputObj.get<T>();
        value.setValue<IlwisData<T>>(outputObj);
        ctx->_results.push_back(outputObj->name());
        symTable.addSymbol(_outputObj->name(),0, _outputObj->ilwisType(),value);

    }
    IIlwisObject _inputObj;
    IIlwisObject _outputObj;

    bool assignGridCoverage();
    bool assignFeatureCoverage();
};
}
}

#endif // ASSIGNMENT_H
