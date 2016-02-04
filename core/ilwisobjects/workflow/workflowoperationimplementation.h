#ifndef WORKFLOWOPERATIONIMPLEMENTATION_H
#define WORKFLOWOPERATIONIMPLEMENTATION_H

#include "workflow.h"

namespace Ilwis {

const QString CONDITION_FAILED = "CONDITION_FAILED";
class WorkflowOperationImplementation: public OperationImplementation
{

public:
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
    Ilwis::OperationImplementation::State prepare(Ilwis::ExecutionContext *ctx, const SymbolTable &);

protected:

private:
    QMap<OVertex, QStringList> _inputArgs;
    QMap<int, bool> _containers;
    QMap<OVertex, std::pair<ExecutionContext*, SymbolTable>> _nodeExecutionContext;
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

};
}

#endif // WORKFLOWOPERATIONIMPLEMENTATION_H
