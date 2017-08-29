#ifndef RUNPYTHONPROGRAM_H
#define RUNPYTHONPROGRAM_H


namespace Ilwis{
namespace PythonScript{

class RunPythonProgram : public RunPython
{
public:
    RunPythonProgram();
    RunPythonProgram(quint64 metaid, const Ilwis::OperationExpression &expr);

    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
    OperationImplementation::State prepare(ExecutionContext *, const SymbolTable &);


};
}
}

#endif // RUNPYTHONPROGRAM_H
