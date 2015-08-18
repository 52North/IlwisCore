#ifndef WORKFLOWOPERATIONIMPLEMENTATION_H
#define WORKFLOWOPERATIONIMPLEMENTATION_H

#include "workflow.h"

namespace Ilwis {
class WorkflowOperationImplementation: public OperationImplementation
{
public:
    WorkflowOperationImplementation(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(Ilwis::ExecutionContext *ctx, const SymbolTable &);

protected:

private:
    QMap<OVertex, QStringList> _inputArgs;
    bool executeFromInputNode(const OVertex &v, ExecutionContext *ctx, SymbolTable &symTable);


};
}

#endif // WORKFLOWOPERATIONIMPLEMENTATION_H
