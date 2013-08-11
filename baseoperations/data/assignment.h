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
        QString name = outputObj->name();
        ctx->_results.push_back(name);
        Resource res = outputObj->source();
        symTable.addSymbol(_outputObj->name(),0, _outputObj->ilwisType(),value);
        if ( name.indexOf(ANONYMOUS_PREFIX) == -1 && res.isValid()) {
            mastercatalog()->addItems({res});
        }

    }
    IIlwisObject _inputObj;
    IIlwisObject _outputObj;
    double _number;
    QString _varName;

    bool assignGridCoverage(ExecutionContext *ctx);
    bool assignFeatureCoverage(ExecutionContext *ctx);
};
}
}

#endif // ASSIGNMENT_H
