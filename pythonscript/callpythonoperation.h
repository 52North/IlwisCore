#ifndef CALLPYTHONOPERATION_H
#define CALLPYTHONOPERATION_H

namespace Ilwis{

class OperationMetaData;

namespace PythonScript {
class CallPythonOperation : public OperationImplementation
{
public:
    CallPythonOperation();
    CallPythonOperation(quint64 metaid, const Ilwis::OperationExpression &expr);
    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);
    static quint64 createMetadata();

    static void addOperation(const QString& name, const QString& filename);

private:
    NEW_OPERATION(CallPythonOperation);

    static std::map<QString, QString> _pythonoperations;
    QString _pythonText;
};
}
}

#endif // CALLPYTHONOPERATION_H
