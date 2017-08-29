#ifndef RUNPYTHON_H
#define RUNPYTHON_H

namespace Ilwis{
namespace PythonScript{

class RunPython : public OperationImplementation
{
public:
    RunPython();
    RunPython(quint64 metaid, const Ilwis::OperationExpression &expr);
    ~RunPython();

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
    OperationImplementation::State prepare(ExecutionContext *, const SymbolTable &);

protected:
    QString _statements;
};
}
}

#endif // RUNPYTHON_H
