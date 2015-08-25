#ifndef WORKFLOWOPERATIONIMPLEMENTATION_H
#define WORKFLOWOPERATIONIMPLEMENTATION_H

#include "workflow.h"

namespace Ilwis {
class WorkflowOperationImplementation: public OperationImplementation
{

public:
    WorkflowOperationImplementation(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *globalCtx, SymbolTable& globalSymTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(Ilwis::ExecutionContext *ctx, const SymbolTable &);

protected:

private:
    QMap<OVertex, QStringList> _inputArgs;
    QMap<OVertex, bool> _nodeExecutionSuccess;
    void copyToContext(const Symbol &symbol, const QString &name, ExecutionContext *ctx, SymbolTable &symTable);
    void parseInputNodeArguments(const QList<OVertex> &inputNodes, const IWorkflow &workflow);
    bool executeInputNode(const OVertex &v, ExecutionContext *ctx, SymbolTable &symTable);
    bool reverseFollowExecutionPath(const OVertex &v, ExecutionContext *ctx, SymbolTable &symTable);


};
}

#endif // WORKFLOWOPERATIONIMPLEMENTATION_H
