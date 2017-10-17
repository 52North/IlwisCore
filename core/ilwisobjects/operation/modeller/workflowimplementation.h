#ifndef WORKFLOWIMPL_H
#define WORKFLOWIMPL_H

#include <mutex>
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "commandhandler.h"
#include "operationmetadata.h"
#include "operation.h"
#include "workflownode.h"
#include "executionnode.h"
#include "workflow.h"
#include "kernel_global.h"

namespace Ilwis {

class Workflow;
typedef IlwisData<Workflow> IWorkflow;


class KERNELSHARED_EXPORT WorkflowImplementation :  public QObject,public OperationImplementation
{
    Q_OBJECT
public:
    WorkflowImplementation();
    WorkflowImplementation(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);
    ExecutionNode& executionNode(const SPWorkFlowNode& node, WorkflowIdMapping &mapping);
    ExecutionNode *executionNode(const SPWorkFlowNode &node) ;

    void wait(const Ilwis::SPWorkFlowNode &node);
    void wakeup();
    void sendData(NodeId nodeId, ExecutionContext *ctx, SymbolTable &symTable, const QString &expr);
    bool stopExecution() const;
    void clearCalculatedValues(qint32 nodeid);
    IWorkflow workflow();

signals:
    void sendMessage(const QString& type, const QString& subtype, const QVariantMap& parameters);
public slots:
    void acceptMessage(const QString& type, const QString& subtype, const QVariantMap& parameters);

private:
    IWorkflow _workflow;
    std::map<NodeId, ExecutionNode> _nodes;
    bool _stepMode = false;
    QMutex _syncMutex;
    std::mutex _lock;
    quint32 _runid=iUNDEF;
    bool _initial = true; // the first node may always run(in stepmode) so we ignore waits
    bool _stopExecution = false;

    void clearCalculatedValues();
    void initStepMode();
};


}

#endif // WORKFLOWIMPL_H
