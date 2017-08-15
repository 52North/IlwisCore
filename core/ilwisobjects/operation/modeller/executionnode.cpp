#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "table.h"
#include "raster.h"
#include "featurecoverage.h"
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
#include "rangenode.h"
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
    auto iter = ctx->_additionalInfo.find("rangeswitch");
    if ( iter != ctx->_additionalInfo.end() ){
        FlowContext fc = (*iter).second.value<FlowContext>();
        if ( fc.second == WorkFlowNode::ntRANGE &&  fc.first != WorkFlowNode::ntRANGE){
            return executeRange(ctx, symTable, workflowImpl, mapping);
        }
    }
    switch(_node->type()) {
    case WorkFlowNode::ntCONDITION:
        ok = executeTest(ctx, symTable, workflowImpl, mapping);break;
    case WorkFlowNode::ntJUNCTION:
        ok = executeJunction(ctx, symTable, workflowImpl, mapping); break;
    case WorkFlowNode::ntOPERATION:
        ok = executeOperation(ctx, symTable, workflowImpl, mapping); break;
    case WorkFlowNode::ntRANGEJUNCTION:
        ok = executeRangeJunction(ctx, symTable, workflowImpl, mapping); break;
    case WorkFlowNode::ntRANGE:
        ok = executeRangeTestNode(ctx, symTable, workflowImpl, mapping); break;
    default:
        return false;
    }
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

bool ExecutionNode::executeRangeTestNode(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, WorkflowIdMapping& mapping)
{
    if ( _node->inputCount() != 1){
        kernel()->issues()->log(TR("Range definition invalid"));
        return false;
    }
    SPLRangeNode rtest = std::static_pointer_cast<RangeNode>(_node);
    SymbolTable symTable2(symTable);
    WorkFlowParameter& parameter = _node->inputRef(0);
    if ( !parameterValue(0).isValid()){
        if (parameter.inputLink()) {
            ExecutionNode& exNode = workflowImpl->executionNode(parameter.inputLink(), mapping);
            ExecutionContext ctx2;
            if ( exNode.execute(&ctx2, symTable2, workflowImpl, mapping)) {
                QString outputName = ctx2._results[parameter.outputParameterIndex()];
                Symbol sym =  symTable2.getSymbol(outputName);
                QVariant val = symTable2.getValue(outputName);

               // rtest->setRangeDefinition(val);
                _parameterValues[0] = val;
            }else{
                return false;
            }
        }
    }
    QVariant currentValue = rtest->currentValue();
    ctx->addOutput(symTable,currentValue,"rangevalue", itANY, Resource());
    return true;
}

bool ExecutionNode::executeRange(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, WorkflowIdMapping& mapping)
{
    std::vector<SPWorkFlowNode> operations = _node->owner()->subnodes("all");
    std::vector<SPWorkFlowNode> outputNodes = Workflow::outputNodes(operations, workflowImpl->workflow().ptr());
    SPLRangeNode range = std::static_pointer_cast<RangeNode>(_node->owner());
    while(range->next()){
        ExecutionContext ctx2;
        SymbolTable symTable2;
        clearCalculatedValues(operations, workflowImpl);
        for(SPWorkFlowNode node : outputNodes ) {
            ExecutionNode& exnode = workflowImpl->executionNode(node, mapping);
            if(!exnode.execute(&ctx2, symTable2, workflowImpl, mapping)){
                return false;
            }
        }
        for(int i=0; i < ctx2._results.size(); ++i){

            Symbol sym = symTable2.getSymbol(ctx2._results[i]);
            QVariant value = sym._var;
            if ( hasType(sym._type, itTABLE)){
                ITable obj = value.value<ITable>();
                ctx->addOutput(symTable,value,obj->name(), obj->ilwisType(), obj->resource() );
            }else  if ( hasType(sym._type, itRASTER)){
                IRasterCoverage obj = value.value<IRasterCoverage>();
                ctx->addOutput(symTable,value,obj->name(), obj->ilwisType(), obj->resource() );
            }else if ( hasType(sym._type, itFEATURE)){
                IFeatureCoverage obj = value.value<IFeatureCoverage>();
                ctx->addOutput(symTable,value,obj->name(), obj->ilwisType(), obj->resource() );
            }else if ( hasType(sym._type, itNUMBER)){
                 ctx->addOutput(symTable, QVariant(value), sUNDEF, itDOUBLE, Resource());
            }else if ( hasType(sym._type, itSTRING)){
                ctx->addOutput(symTable, QVariant(value), sUNDEF, itSTRING, Resource());
           }
        }
    }

    return true;
}

std::pair<WorkFlowNode::NodeTypes, WorkFlowNode::NodeTypes>  ExecutionNode::contextSwitch(const SPWorkFlowNode& sourceNode, const SPWorkFlowNode& targetNode){
    if ( !sourceNode->owner() && !targetNode->owner() )
        return FlowContext(WorkFlowNode::ntNONE,WorkFlowNode::ntNONE);
    if ( !sourceNode->owner() ){
            return FlowContext(WorkFlowNode::ntNONE, targetNode->owner()->type());
    }
    if ( !targetNode->owner() ){
            return FlowContext(sourceNode->owner()->type(),WorkFlowNode::ntNONE);
    }
    return FlowContext(sourceNode->owner()->type(),targetNode->owner()->type());

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
                QVariant fc;
                FlowContext fcTemp = contextSwitch(_node, parameter.inputLink());
                fc.setValue(fcTemp);
                ctx2._additionalInfo["rangeswitch"] = fc;
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

template<class T> void setOutput3(const QVariant& value,ExecutionContext *ctx, SymbolTable &symTable){
    T obj = value.value<T>();
    ctx->addOutput(symTable,value,obj->name(), obj->ilwisType(), obj->resource() );
}


void ExecutionNode::clearCalculatedValues(std::vector<SPWorkFlowNode>& operations, WorkflowImplementation* workflowImpl){
    for(auto& node : operations){
        ExecutionNode* exnode = workflowImpl->executionNode(node)    ;
        if ( exnode)
            exnode->clearScopedCalcutedValues();
    }
}

void ExecutionNode::clearScopedCalcutedValues() {
    for(int i=0; i < _parameterValues.size(); ++i){
        if ( _parameterValues[i].isValid()){
           WorkFlowParameter& parameter = _node->inputRef(i);
           if ( parameter.state() == WorkFlowParameter::pkCALCULATED){
                if ( parameter.inputLink()->owner() == _node->owner()) { // only clean outputs that have been generated in the same container
                    _parameterValues[i] = QVariant();
                } else if ( parameter.inputLink() == _node->owner()  )  { // links to the range itself; output value of the range counter
                    _parameterValues[i] = QVariant();
                }
           }
        }
    }
}

void ExecutionNode::clearCalculatedValues(){
    for(QVariant& v : _parameterValues)
        v = QVariant();
}
