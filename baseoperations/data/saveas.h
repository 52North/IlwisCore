#ifndef SAVEAS_H
#define SAVEAS_H


namespace Ilwis {
namespace BaseOperations {
class SaveAs : public OperationImplementation
{
public:
    SaveAs();
    SaveAs(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &);

    static quint64 createMetadata();
private:
    IIlwisObject _inputObject;
    QString _format;
    QString _provider;
    QUrl _outputUrl;

    NEW_OPERATION(SaveAs);

};
}
}

#endif // SAVEAS_H
