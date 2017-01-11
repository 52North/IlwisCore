#ifndef JUNCTION_H
#define JUNCTION_H

#include "kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT Junction : public WorkFlowNode
{
public:
    Junction();
    Junction(quint64 nodeid);
    QString type() const;


    bool execute(ExecutionContext *ctx, SymbolTable &symTable, const OperationExpression &expression, const std::map<quint64, int> &idmap);
    int inputCount() const;

    void link2trueCase(SPWorkFlowNode trueNode, int parmIndex);
    void link2falseCase(SPWorkFlowNode falseNode, int parmIndex);
    void link2condition(Ilwis::SPWorkFlowNode conditionnode);
    void nodeId(quint64 id);

private:
};
}

#endif // JUNCTION_H
