#ifndef WORKFLOWOPERATIONIMPLEMENTATION_H
#define WORKFLOWOPERATIONIMPLEMENTATION_H

#include <mutex>
#include "kernel.h"
#include "ilwisdata.h"
#include "workflow.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "commandhandler.h"
#include "operation.h"


namespace Ilwis {

struct ExecutionContext;
class SymbolTable;

const QString CONDITION_FAILED = "WORKFLOW_CONDITION_FAILED";
class WorkflowOperationImplementation: public QObject, public OperationImplementation
{
    Q_OBJECT
public:
    WorkflowOperationImplementation(QObject *parent = 0);
    WorkflowOperationImplementation(quint64 metaid, const Ilwis::OperationExpression &expr);

    /*!
     * \brief Executes the workflow
     * \param globalCtx the context to save the output to
     * \param globalSymTable The symbol table to use for saving
     * \return true when succesful false otherhwise
     */
    bool execute(ExecutionContext *globalCtx, SymbolTable& globalSymTable);
    /*!
     * \brief creates a workflowOperationImplemantation from an expression
     * \param metaid The id of the workflow
     * \param expr The expression of the workflow
     * \return the workflowOperationImplemantation
     */
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    /*!
     * \brief Prepares before executing
     * \param ctx The current context
     * \return State of object.
     */
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

signals:
    void sendMessage(const QString& type, const QString& subtype, const QVariantMap& parameters);
public slots:
    void acceptMessage(const QString& type, const QString& subtype, const QVariantMap& parameters);
protected:

private:
    QMap<OVertex, QStringList> _inputArgs;
    QMap<int, bool> _containers;
    QMap<OVertex, std::pair<ExecutionContext*, SymbolTable>> _nodeExecutionContext;
    bool _stepMode; // when in stepmode it pauses after each operation until it gets the order to continue
    bool _wait;
    std::mutex _lock;

    void wait( ExecutionContext *ctx, SymbolTable &symTable);
    /*!
     * \brief Copies output to local context
     * \param symbol Symbol to save
     * \param name Name of output
     * \param ctx Context to save to
     * \param symTable The symbol table
     */
    void copyToContext(const Symbol &symbol, const QString &name, ExecutionContext *ctx, SymbolTable &symTable);
    /*!
     * \brief parses the arguments of the input nodes
     * \param inputNodes The input nodes to parse
     * \param workflow The workflow to parse from
     */
    void parseInputNodeArguments(const QList<OVertex> &inputNodes, const IWorkflow &workflow);
    /*!
     * \brief Executes the vertex
     * \param v The vertex
     * \param ctx Context to save to
     * \param symTable The symbol table
     * \return  true when succesful
     */
    bool executeInputNode(const OVertex &v, ExecutionContext *ctx, SymbolTable &symTable);
    /*!
     * \brief Starts at output nodes and executes vertexes recursive wise up to the input nodes
     * \param v The vertex
     * \param ctx Context to save to
     * \param symTable The symbol table
     * \return  true when succesful
     */
    bool reverseFollowExecutionPath(const OVertex &v, ExecutionContext *ctx, SymbolTable &symTable);
    /*!
     * \brief Checks if vertex is in container. If so, checks all conditions.
     * \param v The vertex
     * \param ctx Context to save to
     * \param symTable The symbol table
     * \return true when all conditions are met. False if otherwise.
     */
    bool checkConditions(const OVertex &v, ExecutionContext *ctx, SymbolTable &symTable);
    bool doCondition(const IOperationMetaData& meta,const OVertex &v, ExecutionContext *ctx, SymbolTable &symTable);
};
}

#endif // WORKFLOWOPERATIONIMPLEMENTATION_H
