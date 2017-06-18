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

ExecutionNode::ExecutionNode(const SPWorkFlowNode& node, WorkflowIdMapping &mapping) : _node(node)
{
    _parameterValues.resize(node->inputCount());
    if ( node->isWorkflow()){
        IWorkflow wf = node->operation().as<Workflow>();
        wf->createMetadata(mapping.offset());
        mapping.advanceOffset(node->inputCount());
    }
}

bool ExecutionNode::execute(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation *workflowImpl, WorkflowIdMapping &mapping)
{
    bool ok;
    switch(_node->type()) {
    case WorkFlowNode::ntCONDITION:
        ok = executeTest(ctx, symTable, workflowImpl, mapping);break;
    case WorkFlowNode::ntJUNCTION:
        ok = executeJunction(ctx, symTable, workflowImpl, mapping); break;
    case WorkFlowNode::ntOPERATION:
        ok = executeOperation(ctx, symTable, workflowImpl, mapping); break;
    case WorkFlowNode::ntRANGE:
        ok = executeRange(ctx, symTable, workflowImpl, mapping); break;
    case WorkFlowNode::ntRANGEJUNCTION:
        ok = executeRangeJunction(ctx, symTable, workflowImpl, mapping); break;
    default:
        return false;
    }
   // if ( )
   // mapping.advanceOffset(parameterCount());
    return ok;
}

void ExecutionNode::unloadInputs(const std::vector<QString>& objects){
    for(const QString& input : objects)    {
        IIlwisObject obj;
        obj.prepare(input);
        if ( obj.isValid()){
            obj->unload();
        }
    }
}

bool ExecutionNode::executeOperation(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, WorkflowIdMapping& mapping)
{
     //auto iter = ctx->_additionalInfo.find("testoperation");
    SymbolTable symTable2(symTable);
    int inputCount = _node->inputCount();
    // for all input parameters
    for(int i=0; i < inputCount; ++i){
        if ( workflowImpl->stopExecution())
            return false;
        WorkFlowParameter& parameter = _node->inputRef(i);
        if ( !parameterValue(i).isValid()){
            if (parameter.inputLink()) {
                ExecutionNode& exNode = workflowImpl->executionNode(parameter.inputLink(), mapping);
                ExecutionContext ctx2;
                if ( exNode.execute(&ctx2, symTable2, workflowImpl, mapping)) {
                    QString outputName = ctx2._results[parameter.outputParameterIndex()];
                    Symbol sym =  symTable2.getSymbol(outputName);
                    QVariant val = symTable2.getValue(outputName);
                    parameter.value("", sym._type);
                    _parameterValues[i] = val;
                }else{
                    return false;
                }
            }
        }
    }
    if ( workflowImpl->stopExecution())
        return false;

    // all input parameters should have a value now so we can start building the expression for this operation
    SPOperationNode opNode = std::static_pointer_cast<OperationNode>(_node);
    IOperationMetaData metadata = opNode->operation();
    QString expr = metadata->name()  + "(";
    QString parms;
    std::vector<QString> unloadableObject;
    for(int i=0; i < inputCount; ++i){
        bool ok = false;
        WorkFlowParameter& inParam = _node->inputRef(i);
        if ( parms != "")
            parms += ",";
        if ( hasType(inParam.valueType(),itILWISOBJECT)){
            QVariant parmValue = mapping.getValue(inParam, *this);
            QString objectname = OperationHelper::variant2string(parmValue, inParam.valueType());
            parms += objectname;
            unloadableObject.push_back(objectname);
        }else {
            if (hasType(inParam.valueType(),itDOUBLE | itFLOAT)) {
                double v = mapping.getValue(inParam,*this).toDouble(&ok);
                if ( ok)
                    parms += QString::number(v);
            }
            if ( !ok && hasType(inParam.valueType(),itINTEGER )) {
                qint64 v = mapping.getValue(inParam,*this).toLongLong(&ok);
                if ( ok)
                    parms += QString::number(v);
            }
            if (  !ok && hasType(inParam.valueType(),itSTRING)){
                parms += "\"" + mapping.getValue(inParam,*this).toString() + "\"";
                ok = true;
            }
            if (!ok)
                parms += mapping.getValue(inParam,*this).toString();
        }
    }
    expr = expr + parms + ")";

    workflowImpl->wait(_node);

    bool ok = commandhandler()->execute(expr,ctx,symTable2);
    unloadInputs(unloadableObject);

    workflowImpl->sendData(_node->id(), ctx, symTable2);
    workflowImpl->wakeup();

    symTable.copyFrom(ctx, symTable2);

    return ok;
}

bool ExecutionNode::executeTest(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, WorkflowIdMapping& mapping)
{

    bool testRestult = true;
    SPCondition condition = std::static_pointer_cast<WorkFlowCondition>(_node);
    // for all tests
    for(int i=0; i < condition->testCount(); ++i){
       const WorkFlowCondition::Test& test = condition->test(i);
       SymbolTable symTableLocal(symTable);
       ExecutionContext ctx;
       ctx._additionalInfo["testoperation"] = true;
       // execute test
       ExecutionNode& exNode = workflowImpl->executionNode(test._operation,mapping);
       if (!exNode.execute(&ctx,symTableLocal,workflowImpl, mapping))
           return false;
       // combine result test with previous tests
       if ( ctx._results.size() == 1){
           Symbol sym = symTableLocal.getSymbol(ctx._results[0]);
           if ( sym._type == itBOOL){
            bool val = sym._var.toBool();
            if ( test._pre != loNONE){
                if ( test._pre == loNOT)
                    val = !val;
            }
            if ( test._post == loAND){
                testRestult &= val;
            }else if ( test._post == loOR)
                testRestult |= val;
           }
       }
    }
    ctx->_results.push_back("testresult") ;
    symTable.addSymbol("testresult",10000, itBOOL, testRestult);

    return true;
}

bool ExecutionNode::executeJunction(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, WorkflowIdMapping& mapping)
{
    // get the parameter that represents the test in the condition. It's value determines which path the junction takes (true, false)
    WorkFlowParameter& testParameter = _node->inputRef(WorkFlowCondition::cpTEST);
     // if the value of the condition (basically the test value
    // is not set we are going to execute the tests of the condition
    // the result of the tests is the "value" of the condition node (true or false)
    // of course this isnt the result of the operations inside the condition; those have their own logic
    if (!parameterValue(WorkFlowCondition::cpTEST).isValid()){
        ExecutionContext ctxLocal;
        SymbolTable symTableLocal(symTable);
        // the value of the parameter comes from the test node, so we follow the inputlink and than execute the test
        // after execution we know the result of the test
        ExecutionNode& tests = workflowImpl->executionNode(testParameter.inputLink(),mapping);
        if (tests.execute(&ctxLocal, symTableLocal,workflowImpl, mapping)){
            // collect the result from the test; the result will be used to determine which branch of the flow is taken
            QString outputName = ctxLocal._results[0];
            QVariant val = symTableLocal.getValue(outputName);
            _parameterValues[WorkFlowCondition::cpTEST] = val;
        }
    }
    ExecutionContext ctxLocal;
    SymbolTable symTableLocal(symTable);
    // determine which link we follow for the flow
    ExecutionNode& exNode = parameterValue(WorkFlowCondition::cpTEST) == "true" ?
                workflowImpl->executionNode(_node->inputRef(WorkFlowCondition::cpTRUECASE).inputLink(),mapping) :
                workflowImpl->executionNode(_node->inputRef(WorkFlowCondition::cpFALSECASE).inputLink(),mapping);

    exNode.execute(ctx, symTableLocal,workflowImpl, mapping);
    symTable.copyFrom(ctx, symTableLocal);

    return true;
}

bool ExecutionNode::executeContent(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, WorkflowIdMapping& mapping){
    SymbolTable symTable2(symTable);
    int inputCount = _node->inputCount();
    for(int i=0; i < inputCount; ++i){
        WorkFlowParameter& parameter = _node->inputRef(i);
        if ( !parameterValue(i).isValid()){
            ExecutionContext ctx2;
            if (parameter.inputLink()) {
                ExecutionNode& exNode = workflowImpl->executionNode(parameter.inputLink(),mapping);
                if ( exNode.execute(&ctx2, symTable2,workflowImpl, mapping)) {
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

QVariant ExecutionNode::parameterValue(int parmIndex) const
{
    if ( _node->inputRef(parmIndex).state() == WorkFlowParameter::pkCALCULATED)
        return _parameterValues[parmIndex];
    return _node->inputRef(parmIndex).value();
}

int ExecutionNode::parameterCount() const
{
    return _parameterValues.size();
}

bool ExecutionNode::executeRangeJunction(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, WorkflowIdMapping& mapping){
    return executeJunction(ctx,symTable, workflowImpl, mapping)    ;
}

bool ExecutionNode::executeRange(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, WorkflowIdMapping& mapping)
{
    while( next()){
        if(!executeContent(ctx,symTable, workflowImpl, mapping))
            return false;
    }

    return true;
}

void ExecutionNode::clearCalculatedValues(){
    for(QVariant& v : _parameterValues)
        v = QVariant();
}
