#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "operationExpression.h"
#include "commandhandler.h"
#include "operationmetadata.h"
#include "operation.h"
#include "mastercatalog.h"
#include "operationhelper.h"
#include "workflownode.h"
#include "workflow.h"
#include "operationnode.h"
#include "conditionNode.h"
#include "executionnode.h"
#include "workflowimplementation.h"

using namespace Ilwis;

ExecutionNode::ExecutionNode(){

}

ExecutionNode::ExecutionNode(const SPWorkFlowNode& node) : _node(node)
{
    _parameterValues.resize(node->inputCount(),sUNDEF);
}

bool ExecutionNode::execute(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation *workflowImpl, const OperationExpression &expression, const std::map<quint64, int> &idmap)
{
    switch(_node->type()) {
    case WorkFlowNode::ntCONDITION:
        return executeCondition(ctx, symTable, workflowImpl, expression, idmap);
    case WorkFlowNode::ntJUNCTION:
        return executeJunction(ctx, symTable, workflowImpl, expression, idmap);
    case WorkFlowNode::ntOPERATION:
        return executeOperation(ctx, symTable, workflowImpl, expression, idmap);
    case WorkFlowNode::ntLOOP:
        return executeLoop(ctx, symTable, workflowImpl, expression, idmap);
    default:
        return false;
    }
    return false;
}

bool ExecutionNode::executeOperation(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, const OperationExpression &expression, const std::map<quint64, int> &idmap)
{
    auto getValue = [&](const WorkFlowParameter& parm, const OperationExpression& expression, const std::map<quint64, int>& idmap)-> QVariant{
        auto iter = idmap.find(parm.id());
        if  ( iter != idmap.end())       {
            int idx = (*iter).second;
            return expression.parm(idx).value();
        }
        return parameterValue(parm.order());
    };
    SymbolTable symTable2(symTable);
    int inputCount = _node->inputCount();
    for(int i=0; i < inputCount; ++i){
        WorkFlowParameter& parameter = _node->inputRef(i);
        if ( parameterValue(i) == sUNDEF){
            ExecutionContext ctx2;
            if (parameter.inputLink()) {
                ExecutionNode& exNode = workflowImpl->executionNode(parameter.inputLink());
                if ( exNode.execute(&ctx2, symTable2, workflowImpl, expression, idmap)) {
                    QString outputName = ctx2._results[parameter.outputParameterIndex()];
                    QVariant val = symTable2.getValue(outputName);
                    QString sval = OperationHelper::variant2string(val, symTable2.getSymbol(outputName)._type);
                    parameter.value("", symTable2.getSymbol(outputName)._type);
                    _parameterValues[i] = sval;
                }else
                    return false;
            }
        }
    }
    SPOperationNode opNode = std::static_pointer_cast<OperationNode>(_node);
    IOperationMetaData metadata = opNode->operation();
    QString expr = metadata->name()  + "(";
    QString parms;
    for(int i=0; i < inputCount; ++i){
        WorkFlowParameter& inParam = _node->inputRef(i);
        if ( parms != "")
            parms += ",";
        if ( hasType(inParam.valueType(),itILWISOBJECT)){
            parms += getValue(inParam, expression, idmap).toString();
        }else if ( hasType(inParam.valueType(),itINTEGER )) {
            parms += QString::number(getValue(inParam,expression, idmap).toLongLong());
        } else if ( hasType(inParam.valueType(),itDOUBLE | itFLOAT)) {
            parms += QString::number(getValue(inParam,expression, idmap).toDouble());
        } else if (hasType(inParam.valueType(),itSTRING)){
            parms += "\"" + getValue(inParam,expression, idmap).toString() + "\"";
        }else
            parms += getValue(inParam,expression, idmap).toString();
    }
  // expr = "script " + outNames +  "=" + expr + parms + ")";
    expr = expr + parms + ")";
    bool ok = commandhandler()->execute(expr,ctx,symTable2);
    symTable.copyFrom(ctx, symTable2);

    return ok;
}

bool ExecutionNode::executeCondition(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, const OperationExpression &expression, const std::map<quint64, int> &idmap)
{

    bool testRestult = true;
    SPCondition condition = std::static_pointer_cast<WorkFlowCondition>(_node);
    for(int i=0; i < condition->testCount(); ++i){
       const WorkFlowCondition::Test& test = condition->test(i);
       SymbolTable symTableLocal(symTable);
       ExecutionContext ctx;
       ExecutionNode& exNode = workflowImpl->executionNode(test._operation);
       if (!exNode.execute(&ctx,symTable,workflowImpl, expression, idmap))
           return false;
       if ( ctx._results.size() == 1){
           Symbol sym = symTableLocal.getSymbol(ctx._results[0]);
           if ( sym._type == itBOOL){
            bool val = sym._var.toBool();
            if ( test._pre != loNONE){
                if ( test._pre == loNOT)
                    val = !val;
            }
            if ( test._post != loAND){
                testRestult &= val;
            }else
                testRestult |= val;
           }
       }
    }
    ctx->_results.push_back("testresult") ;
    symTable.addSymbol("testresult",10000, itBOOL, testRestult);

    return true;
}

bool ExecutionNode::executeJunction(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, const OperationExpression &expression, const std::map<quint64, int> &idmap)
{
    WorkFlowParameter& condParm = _node->inputRef(0);
     // if the value of the condition (basically the test value
    // is not set we are going to execute the tests of the condition
    // the result of the tests is the "value" of the condition node (true or false)
    // of course this isnt the result of the operations inside the condition; those have their own logic
    if (parameterValue(0) == sUNDEF){
        ExecutionContext ctxLocal;
        SymbolTable symTableLocal(symTable);
        ExecutionNode& exNode = workflowImpl->executionNode(condParm.inputLink());
        if (exNode.execute(&ctxLocal, symTableLocal,workflowImpl, expression, idmap)){
            QString outputName = ctxLocal._results[0];
            QVariant val = symTableLocal.getValue(outputName);
            QString sval = OperationHelper::variant2string(val, symTableLocal.getSymbol(outputName)._type);
            condParm.value("", symTableLocal.getSymbol(outputName)._type);
            _parameterValues[0] = sval;
        }
    }
    ExecutionContext ctxLocal;
    SymbolTable symTableLocal(symTable);
    ExecutionNode& exNode = parameterValue(0) == "true" ?
                workflowImpl->executionNode(_node->inputRef(1).inputLink()) :
                workflowImpl->executionNode(_node->inputRef(2).inputLink());

    exNode.execute(ctx, symTableLocal,workflowImpl, expression, idmap);
    symTable.copyFrom(ctx, symTableLocal);

    return true;
}

bool ExecutionNode::executeContent(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, const OperationExpression &expression, const std::map<quint64, int> &idmap){
    SymbolTable symTable2(symTable);
    int inputCount = _node->inputCount();
    for(int i=0; i < inputCount; ++i){
        WorkFlowParameter& parameter = _node->inputRef(i);
        if ( parameterValue(i) == sUNDEF){
            ExecutionContext ctx2;
            if (parameter.inputLink()) {
                ExecutionNode& exNode = workflowImpl->executionNode(parameter.inputLink());
                if ( exNode.execute(&ctx2, symTable2,workflowImpl, expression, idmap)) {
                    QString outputName = ctx2._results[parameter.outputParameterIndex()];
                    QVariant val = symTable2.getValue(outputName);
                    QString sval = OperationHelper::variant2string(val, symTable2.getSymbol(outputName)._type);
                    parameter.value(sval, symTable2.getSymbol(outputName)._type);
                    _parameterValues[i] = sval;
                }else
                    return false;
            }
        }
    }
    return true;
}

bool ExecutionNode::next(){
    return false;
}

QString ExecutionNode::parameterValue(int parmIndex) const
{
    if ( _node->inputRef(parmIndex).state() == WorkFlowParameter::pkCALCULATED)
        return _parameterValues[parmIndex];
    return _node->inputRef(parmIndex).value();
}

bool ExecutionNode::executeLoop(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, const OperationExpression &expression, const std::map<quint64, int> &idmap)
{
    while( next()){
        if(!executeContent(ctx,symTable, workflowImpl, expression, idmap))
            return false;
    }

    return true;
}

void ExecutionNode::clearCalculatedValues(){
    for(QString& v : _parameterValues)
        v = sUNDEF;
}
