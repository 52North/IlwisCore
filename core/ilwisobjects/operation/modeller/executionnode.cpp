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
#include "rangejunctionnode.h"
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
        if ((fc.second._nodeType == WorkFlowNode::ntRANGE && fc.first._nodeType != WorkFlowNode::ntRANGE) ||
               (fc.second._nodeType == WorkFlowNode::ntRANGE && fc.first._nodeType == WorkFlowNode::ntRANGE && fc.first._nodeid != fc.second._nodeid)){
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

                  rtest->setRangeDefinition(val.toString(), workflowImpl->workflow().ptr());
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

void ExecutionNode::setInputJunction(ExecutionContext *ctx, SymbolTable &symTable,const SPWorkFlowNode& operation, WorkflowImplementation* workflowImpl, WorkflowIdMapping& mapping){
    if ( ctx->_results.size() == 0)
        return;

    std::vector<SPWorkFlowNode> junctions =_node->owner()->subnodes("junctions");
    QString outputName = ctx->_results[0];
    QVariant val = symTable.getValue(outputName);
    //check for all junctions in this range if they are connected to the operation. If so the output of operations should be used as input for the junction
    for(SPWorkFlowNode node : junctions){
        // see if a junction gets input of the operation
        SPWorkFlowNode linkedOutput = node->inputRef(RangeNode::rpINPUT).inputLink();
        if ( linkedOutput && linkedOutput->id() == operation->id()){
            // if so store its output as input in the junction
            ExecutionNode& ex = workflowImpl->executionNode(node, mapping);
            ex.parameterValue(0,val);
        }
    }
}

bool ExecutionNode::executeRange(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, WorkflowIdMapping& mapping)
{
    std::vector<SPWorkFlowNode> operations = _node->owner()->subnodes("operations");
    SPLRangeNode range = std::static_pointer_cast<RangeNode>(_node->owner());
    if ( range->input(0).inputLink()){ // initializing the range if it has input from another node
        ExecutionNode& exnode = workflowImpl->executionNode(range, mapping);
        ExecutionContext ctxLocal;
        SymbolTable symTableLocal;
        if(!exnode.execute(&ctxLocal, symTableLocal, workflowImpl, mapping)){
            return false;
        }
    }

    ExecutionNode *exNodeExisting = workflowImpl->executionNode(_node);
    // as the output nodes always need to generate output the symbol table should be have some symbols
    // if not, this is the first run of the range. If it has values we can pick up the values and continue
    if (exNodeExisting->symbolTableRef().symbols().size() == 0){
        // get all the nodes whose outputs are not used in the list of operations of the range; this means they go outside
        // and thus are outputs of the range
        std::vector<SPWorkFlowNode> outputNodes = workflowImpl->workflow()->leafNodes(_node->owner()->id());
        while(range->next()){

            // clear all outputs of all objects that are internal to the range. In each run we want to use a clear scope
            clearCalculatedValues(operations, workflowImpl);
            for(SPWorkFlowNode node : outputNodes ) {
                ExecutionContext ctx2;
                ExecutionNode& exnode = workflowImpl->executionNode(node, mapping);
                if(!exnode.execute(&ctx2, exnode.symbolTableRef(), workflowImpl, mapping)){
                    return false;
                }
                setInputJunction(&ctx2, exnode.symbolTableRef(), node,workflowImpl, mapping);
                //remove results of a previous run of the loop.
                exnode.symbolTableRef().removeAllBut(ctx2._results);
            }
        }
    }

    // the symbol table contains all outputs of the range for this output node
    const QHash<QString, Symbol> symbols = exNodeExisting->symbolTableRef().symbols();
    // add them to the output of this execution node.
    for(const Symbol& sym : symbols){
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


    return true;
}
//checks the nature of a flow; if it goes inside a container or leaves a container or jumps from container to container
FlowContext  ExecutionNode::contextSwitch(const SPWorkFlowNode& sourceNode, const SPWorkFlowNode& targetNode){
    if ( !sourceNode){ // special case at start of worklfow
        IdTypePair ip1(i64UNDEF, WorkFlowNode::ntNONE);
        IdTypePair ip2(targetNode->id(), targetNode->owner() ? targetNode->owner()->type() : targetNode->type());
        return FlowContext(ip1, ip2);
    }
    // no owners; nodes outside any container
    if ( !sourceNode->owner() && !targetNode->owner() ){
        IdTypePair ip1(sourceNode->id(), WorkFlowNode::ntNONE);
        IdTypePair ip2(targetNode->id(), WorkFlowNode::ntNONE);
        return FlowContext(ip1, ip2);
    }
    //only the source has no owner, so a flow from outside to inside a container
    if ( !sourceNode->owner() ){
        IdTypePair ip1(sourceNode->id(), WorkFlowNode::ntNONE);
        IdTypePair ip2(targetNode->id(), targetNode->owner()->type());
        return FlowContext(ip1, ip2);
    }
    //only the target has no owner. so a flow from inside a container to outside a container
    if ( !targetNode->owner() ){
        IdTypePair ip1(sourceNode->id(), sourceNode->owner()->type());
        IdTypePair ip2(targetNode->id(), WorkFlowNode::ntNONE);
        return FlowContext(ip1, ip2);
    }
    //both have an owner, so the two nodes are in the same container
    IdTypePair ip1(sourceNode->owner()->id(), sourceNode->owner()->type());
    IdTypePair ip2(targetNode->owner()->id(), targetNode->owner()->type());
    return FlowContext(ip1, ip2);

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
        // if there is a parameter value it has been calculated by other uses of this node so ne recalculation needed
        // the following code tries to determine the value of the parameter even if that parameter a link to the rest of the system.
        // a value is either a fixed value, a input(free) value(in this context== fixed value) or a linked value meaning it is the result of a calculation
        if ( !parameterValue(i).isValid()){
            if (parameter.inputLink()) {
                //create an execution node for the link
                ExecutionNode& exNode = workflowImpl->executionNode(parameter.inputLink(), mapping);
                ExecutionContext ctx2;
                QVariant fc;
                //determine the nature of the link between the two nodes
                FlowContext fcTemp = ExecutionNode::contextSwitch(_node, parameter.inputLink());
                fc.setValue(fcTemp);
                ctx2._additionalInfo["rangeswitch"] = fc;
                // execute the link, meaning it moves the focus to the links node and determines there what to be done
                // on return the ctx/symtable combo will contain the results generated by this execute and the current node
                // can continue building the context needed for its own execution (at the end of this method)
                if ( exNode.execute(&ctx2, symTable2, workflowImpl, mapping)) {
                    if ( ctx2._results.size() > 0 && parameter.outputParameterIndex() < ctx2._results.size()){
                        // store the results of the link and make it available to the building of the expression
                        QString outputName = ctx2._results[parameter.outputParameterIndex()];
                        Symbol sym =  symTable2.getSymbol(outputName);
                        QVariant val = symTable2.getValue(outputName);
                        //parameter.value("", sym._type);
                        parameter.valueType( sym._type);
                        _parameterValues[i] = val;
                        if ( parameter.inputLink()->type() == WorkFlowNode::ntRANGEJUNCTION){
                            ExecutionNode& ex = workflowImpl->executionNode(parameter.inputLink(), mapping);
                            ex.parameterValue(0,val);
                        }
                    }
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
            if ( objectname == ""){
                kernel()->issues()->log(TR("An object could not be resolved to a meaningful name, execution halted"));
                return false;
            }
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
                QString v = mapping.getValue(inParam,*this).toString().trimmed();
                bool needsquotes = false;
                if ( v[0] != '\"'){ // don't place quotes were they are already present
                    needsquotes = v.indexOf(QRegExp("[ ,)(\"\']")) != -1;
                    if (!needsquotes){
                        QString key = "pin_" + QString::number(i) + "_needsquotes";
                        needsquotes = metadata->resource().hasProperty(key) && metadata->resource()[key].toBool();
                    }
                }
                if ( needsquotes)
                    v = "\"" +  v + "\"";
                parms += v;
                ok = true;
            }
            if (!ok)
                parms += mapping.getValue(inParam,*this).toString();
        }
    }
    expr = expr + parms + ")";

    workflowImpl->wait(_node);

    ExecutionContext ctx2;
    bool ok = commandhandler()->execute(expr,&ctx2,symTable2);
    unloadInputs(unloadableObject);

    workflowImpl->sendData(_node->id(), &ctx2, symTable2, expr);
    workflowImpl->wakeup();

    for(auto res : ctx2._results)
        ctx->_results.push_back(res);
    symTable.copyFrom(&ctx2, symTable2);

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

void ExecutionNode::parameterValue(int parmIndex, const QVariant &v)
{
    if ( parmIndex < _parameterValues.size()){
        _parameterValues[parmIndex] = v;
    }
}

int ExecutionNode::parameterCount() const
{
    return _parameterValues.size();
}

SymbolTable& ExecutionNode::symbolTableRef()
{
    return _symTable;
}

bool ExecutionNode::executeLink(ExecutionContext *ctx, SymbolTable &symTable, WorkFlowParameter& parameter,WorkflowImplementation* workflowImpl, WorkflowIdMapping& mapping){
    QVariant v = parameterValue(0);
    if ( !v.isValid() || v.toString() == sUNDEF){
        if (parameter.inputLink()) {
            ExecutionNode& exNode = workflowImpl->executionNode(parameter.inputLink(), mapping);
            SymbolTable symTableLocal(symTable);
            QVariant fc;
            FlowContext fcTemp = ExecutionNode::contextSwitch(_node, parameter.inputLink());
            fc.setValue(fcTemp);
            ctx->_additionalInfo["rangeswitch"] = fc;
            if ( exNode.execute(ctx, symTableLocal, workflowImpl, mapping)) {
                QString outputName = ctx->_results[parameter.outputParameterIndex()];
                Symbol sym =  symTableLocal.getSymbol(outputName);
                QVariant val = symTableLocal.getValue(outputName);
                parameter.value("", sym._type);
                _parameterValues[0] = val;
            }else{
                return false;
            }
            symTable.copyFrom(ctx, symTableLocal);
        }
    }else {
        ctx->_results.push_back("junctioninput");
        symTable.addSymbol("junctioninput",0,TypeHelper::variant2type(v), v);
    }

    return true;
}

bool ExecutionNode::executeRangeJunction(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, WorkflowIdMapping& mapping){
    if ( workflowImpl->stopExecution())
        return false;
    WorkFlowParameter& parameter = _node->inputRef(RangeNode::rpINITIALINPUT);

    QVariant v = parameterValue(0);
    if ( !v.isValid() || v.toString() == sUNDEF){
        if(! executeLink(ctx, symTable, parameter, workflowImpl, mapping))
            return false;
        _node->inputRef(0).state(WorkFlowParameter::pkCALCULATED);
    }else {
        WorkFlowParameter& parameter2 = _node->inputRef(RangeNode::rpINPUT);
        if(! executeLink(ctx, symTable, parameter2, workflowImpl, mapping))
            return false;
    }
    return true;
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
