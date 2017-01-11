#include <functional>
#include <future>
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "table.h"
#include "raster.h"
#include "featurecoverage.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "workflownode.h"
#include "conditionNode.h"
#include "junctionNode.h"
#include "workflowimplementation.h"
#include "modeller/workflow.h"

using namespace Ilwis;

Workflow::Workflow()
{

}

Workflow::Workflow(const Resource &resource) : OperationMetaData(resource)
{

}

std::vector<SPWorkFlowNode> Workflow::outputNodes(const std::vector<SPWorkFlowNode> graph)
{
    std::set<NodeId> usedNodes;
    for(const auto& item : graph){

        if ( item->type() == "operationnode"){ // only operations can have direct inputs; the inputs of a condition or junction are always set by the system
            for(int i=0; i < item->inputCount(); ++i){
                if ( item->inputRef(i).inputLink())
                    usedNodes.insert(item->inputRef(i).inputLink()->id());
            }
        }
    }
    //if a node is not used as input somewhere it must be an output
    std::vector<SPWorkFlowNode> nodes;
    for(auto node : graph){
        auto iter = usedNodes.find(node->id());
        if (iter == usedNodes.end()){
            nodes.push_back(node);
        }
    }
    return nodes;
}

std::vector<SPWorkFlowNode> Workflow::outputNodes()
{
    return outputNodes(_graph);
}

void Workflow::setFixedParameter(const QString &data, NodeId nodeId, qint32 parmIndex)
{
    SPWorkFlowNode node = nodeById(nodeId);
    if (node){
        if (  parmIndex < node->inputCount()){
            WorkFlowParameter& param = node->inputRef(parmIndex);
            IlwisTypes tp = OperationHelper::determineType(data);
            param.value(data, tp);
        }
    }

}
void Workflow::addConditionFlow(NodeId fromNode, NodeId toNode, qint32 testIndex, qint32 inParmIndex, qint32 outParmIndex, int attachRctIndxFrom, int attachRctIndxTo){
    SPWorkFlowNode from = nodeById(fromNode);
    SPWorkFlowNode to = nodeById(toNode);
    if ( from && to){
        std::shared_ptr<WorkFlowCondition> condition = std::static_pointer_cast<WorkFlowCondition>(to);
        if ( testIndex < condition->testCount()){
            WorkFlowCondition::Test test = condition->test(testIndex);
            addFlow(fromNode, test._operation->id(), inParmIndex, outParmIndex, attachRctIndxFrom, attachRctIndxTo);
        }
    }
}

void Workflow::addFlow(NodeId fromNode, NodeId toNode, qint32 inParmIndex, qint32 outParmIndex, int attachRctIndxFrom, int attachRctIndxTo)
{
    SPWorkFlowNode from = nodeById(fromNode);
    SPWorkFlowNode to = nodeById(toNode);
    if ( from && to){
        if ( inParmIndex < to->inputCount()){
            to->inputRef(inParmIndex).inputLink(from,outParmIndex);
            to->inputRef(inParmIndex).attachement(attachRctIndxFrom, true);
            to->inputRef(inParmIndex).attachement(attachRctIndxTo, false);
        }
    }
}

void Workflow::removeFlow(NodeId toNode, qint32 parameterIndex)
{
    SPWorkFlowNode node = nodeById(toNode);
    if (node){
        if (  parameterIndex < node->inputCount()){
            WorkFlowParameter& param = node->inputRef(parameterIndex);
            param.inputLink(SPWorkFlowNode());
        }
    }
}

void Workflow::addJunctionFlow(int junctionIdTo, const QString &operationIdFrom, int paramIndex, int rectFrom, int rectTo, bool truecase)
{
    SPWorkFlowNode node = nodeById(junctionIdTo);
    SPWorkFlowNode operationFrom = nodeById(operationIdFrom.toULongLong());
    std::shared_ptr<Junction> junction = std::static_pointer_cast<Junction>(node);
    if ( truecase){

        junction->link2trueCase(operationFrom, paramIndex);
        junction->inputRef(1).attachement(rectFrom, true);
        junction->inputRef(1).attachement(rectTo, false);
    }else{
        junction->link2falseCase(operationFrom, paramIndex);
        junction->inputRef(2).attachement(rectFrom, true);
        junction->inputRef(2).attachement(rectTo, false);
    }

}

qint32 Workflow::operationInputParameterCount(NodeId nodeId)
{
    SPWorkFlowNode node = nodeById(nodeId);
    if (node){
        IOperationMetaData metadata = node->operation();
        if ( metadata.isValid())
            return metadata->inputParameterCount();
    }
    return iUNDEF;
}

qint32 Workflow::operationOutputParameterCount(NodeId nodeId)
{
    SPWorkFlowNode node = nodeById(nodeId);
    if (node){
        IOperationMetaData metadata = node->operation();
        if ( metadata.isValid())
            return metadata->outputParameterCount();
    }
    return iUNDEF;
}

bool Workflow::isParameterValueDefined(NodeId nodeId, qint32 parameterIndex) const
{
    const SPWorkFlowNode node = nodeById(nodeId);
    if (node){
        if (  parameterIndex < node->inputCount()){
           return node->inputRef(parameterIndex).state() != WorkFlowParameter::pkFREE;
        }
    }
    return false;
}

double Workflow::scale() const
{
    return _scale;
}

void Workflow::scale(double s)
{
    if ( s > 0)
        _scale = s;
}

std::pair<int, int> Workflow::translation() const
{
    return _translation;
}

void Workflow::translation(double x, double y)
{
    _translation = std::pair<int, int>(x,y);
}

quint32 Workflow::generateId()
{
    return ++_idCounter;
}

void Workflow::updateIdCounter()
{
    Workflow::ExecutionOrder order = executionOrder();
    std::vector<SPWorkFlowNode> nodes = order._independentOrder;
    qint64 maxid = 0;

    for(SPWorkFlowNode node : order._independentOrder){
        maxid = Ilwis::max(maxid, (qint64)node->id());
    }
    for(auto item : order._dependentOrder){
        nodes = item.second;
        for(const SPWorkFlowNode& node : nodes){
            maxid = Ilwis::max(maxid, node->id());
        }
    }
    _idCounter = maxid + 1;
}

const std::vector<SPWorkFlowNode> &Workflow::graph() const
{
    return _graph;
}

std::vector<WorkFlowParameter> Workflow::freeInputParameters() const
{
   std::vector<WorkFlowParameter> result;
   for(const auto& item : _graph){
        for(int i=0; i < item->inputCount(); ++i){
            WorkFlowParameter& p = item->inputRef(i);
            if ( p.state() == WorkFlowParameter::pkFREE){
                result.push_back(p);
            }
        }
   }
   return result;
}

std::vector<SPOperationParameter> Workflow::freeOutputParameters() const
{
    auto CheckLinks =[](WorkFlowParameter& p,std::map<NodeId, std::vector<SPOperationParameter>>& outparams)->void {
        // if an outputParameterIndex is defined it can be scrapped from the list of potential free parameters
        if ( p.outputParameterIndex() != iUNDEF){
            // junctionnodes are not regular operations and can be skipped; they are always intermediaries
            if ( p.inputLink()->type() != "junctionnode")
                if ( outparams.find(p.inputLink()->id()) != outparams.end())
                    outparams[p.inputLink()->id()][p.outputParameterIndex()] = SPOperationParameter();
        }
    };
    std::vector<SPOperationParameter> result;
    std::map<NodeId, std::vector<SPOperationParameter>> outparams;
    for(const auto& item : _graph){
        if (item->type() == "operationnode"){
            IOperationMetaData md = item->operation();
            if ( md.isValid()){
                outparams[item->id()] = md->getOutputParameters();
            }
        }
    }
    //we have collected a list of all outputs and are now going to check which ones are linked
    // linked to a previous input and thus not being free; they are scrapped from the list by
    // making its value invalied
    for(const auto& item : _graph){
        if (item->type() == "conditionnode")
            continue;

        if ( item->type() == "junctionnode"){
             //note that the true case (2 == false) isnt under consideration as
            // its output always goes to a junction so it will never appear in the outparams
            // outparams are build on 'visible'nodes and the inside of a condition is not visibile
            // to the outside and the true value comes from there
             WorkFlowParameter& p = item->inputRef(2);
             CheckLinks(p, outparams);

        }else if ( item->type() == "operationnode"){
            for(int i=0; i < item->inputCount(); ++i){
                WorkFlowParameter& p = item->inputRef(i);
                CheckLinks(p, outparams);
           }
        }

    }
    //collect now all parameters which have not been invalidated; these are the output parameters that are free and must be entered from the outside
    for(auto item : outparams){
        for(auto parm : item.second) {
            if ( parm)    {
                result.push_back(parm);
            }
        }
    }
    return result;
}

void Workflow::reverseExecutionOrder(SPWorkFlowNode node,std::vector<SPWorkFlowNode>& executionOrder , std::set<SPWorkFlowNode>& usedNodes){
    auto iter = usedNodes.find(node);
    if (iter == usedNodes.end()){
        usedNodes.insert(node);

        executionOrder.push_back(node);
        for(int i=0; i < node->inputCount(); ++i){
            if ( node->inputRef(i).inputLink()){
                Workflow::reverseExecutionOrder(node->inputRef(i).inputLink(), executionOrder, usedNodes);
            }
        }
    }
}

Workflow::ExecutionOrder Workflow::executionOrder(){
    return  Workflow::executionOrder(_graph);
}

Workflow::ExecutionOrder Workflow::executionOrder(std::vector<SPWorkFlowNode>& graph) {

   ExecutionOrder executionOrder;
   std::vector<SPWorkFlowNode> nodes = Workflow::outputNodes(graph);
   std::set<SPWorkFlowNode> usedNodes;

    for(auto node : nodes ){
        Workflow::reverseExecutionOrder(node,  executionOrder._independentOrder, usedNodes);
    }

   for(auto node : nodes ){
       if ( node->type() == "conditionnode"){
          std::vector<SPWorkFlowNode> conditionOperations = node->subnodes("operations") ;
          std::vector<SPWorkFlowNode> outnodesCondition = Workflow::outputNodes(conditionOperations);
          std::vector<SPWorkFlowNode> order;
          for(auto node : outnodesCondition ){
              Workflow::reverseExecutionOrder(node, order, usedNodes);
          }
          executionOrder._dependentOrder[node->id()] = order;
       }
   }

    return executionOrder;
}

NodeId Workflow::addNode(SPWorkFlowNode node, NodeId parent)
{
    auto iter = std::find(_graph.begin(), _graph.end(), node);
    if ( iter == _graph.end() ){
        if ( node->id() == i64UNDEF)
            node->nodeId(generateId());

        if ( parent != i64UNDEF){
            SPWorkFlowNode parentNode = nodeById(parent);
            if ( parentNode){
                parentNode->addSubNode(node, "operations");
            }
        }else
           _graph.push_back(node);
        return node->id();
    }
    return i64UNDEF;
}

SPWorkFlowNode Workflow::nodeById(NodeId id)
{
    for(auto node : _graph){
        if ( node->id() == id)
            return node;
        //it might be part of a condition
        if ( node->type() == "conditionnode"){
            std::vector<SPWorkFlowNode> subnodes = node->subnodes("all");
            for(auto subnode : subnodes)
                if ( subnode->id() == id)
                    return subnode;
        }
    }
    return SPWorkFlowNode();

}

const SPWorkFlowNode Workflow::nodeById(NodeId id) const
{
    for(auto node : _graph){
        if ( node->id() == id)
            return node;
        //it might be part of a condition
        if ( node->type() == "conditionnode"){
            std::vector<SPWorkFlowNode> subnodes = node->subnodes("all");
            for(auto subnode : subnodes)
                if ( subnode->id() == id)
                    return subnode;
        }
    }
    return SPWorkFlowNode();

}


void Workflow::removeNode(NodeId id)
{
    for(auto node : _graph){
        if ( node->id() == id){
            for(auto linkedNode : _graph){
                int n;
                if ( (n =linkedNode->inputCount()) > 0){
                    for(int i=0; i < n; ++i)    {
                        WorkFlowParameter& param = linkedNode->inputRef(i);
                        if ( param.inputLink()){
                            if ( param.inputLink()->id() == id){
                                param.inputLink(SPWorkFlowNode());
                            }
                        }
                    }
                }
            }
        }
    }
}

std::map<quint64, int> Workflow::parmid2order() const {
    return _parmid2order;
}

quint64 Workflow::createMetadata()
{
    QString opname = name();
    OperationResource operation = resource();
    opname.remove(".ilwis");
    if (!operation.isValid())
        operation = OperationResource(QUrl("ilwis://operations/" + opname));
    std::vector<WorkFlowParameter> inputparams = freeInputParameters();
    std::vector<SPOperationParameter> outparams = freeOutputParameters();
    int count = 0;
    _parmid2order.clear();
    QString syntax = opname + "(";
    operation.setInParameterCount({inputparams.size()});
    for(WorkFlowParameter parm : inputparams){
        _parmid2order[parm.id()] = count;
        operation.addInParameter(count,parm.valueType(), parm.label(),parm.description());
        if ( count != 0 )
            syntax += ",";
        syntax += parm.syntax();
        ++count;
    }
    syntax += ")";
    operation.setSyntax(syntax);

    count = 0;
    operation.setOutParameterCount({outparams.size()});
    for(SPOperationParameter parm : outparams){
        operation.addOutParameter(count++,parm->type(), parm->name(),parm->description());
    }
    resourceRef() = operation;
    mastercatalog()->addItems({operation});
    Operation::registerOperation(operation.id(),WorkflowImplementation::create);
    return operation.id();
}

IlwisTypes Workflow::ilwisType() const
{
    return itWORKFLOW;
}
