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
#include "rangenode.h"
#include "executionnode.h"
#include "workflowimplementation.h"
#include "modeller/workflow.h"

using namespace Ilwis;

Workflow::Workflow()
{

}

Workflow::Workflow(const Resource &resource) : OperationMetaData(resource)
{
}

std::vector<SPWorkFlowNode> Workflow::outputNodes(const std::vector<SPWorkFlowNode> graph,Workflow *flow)
{
    std::set<NodeId> usedNodes;
    std::set<NodeId> rangeNodes;
    for(const auto& item : graph){

        if ( item->type() == WorkFlowNode::ntOPERATION){ // only operations can have direct inputs; the inputs of a  junction are always set by the system,
            //condtions have input through their tests
            for(int i=0; i < item->inputCount(); ++i){
                if ( item->inputRef(i).inputLink())
                    usedNodes.insert(item->inputRef(i).inputLink()->id());
            }
        }else if ( item->type() == WorkFlowNode::ntJUNCTION){
            if ( item->inputRef(1).inputLink()){ // link to the a true output of an operation in the false branch
                usedNodes.insert(item->inputRef(1).inputLink()->id());
            }
            if ( item->inputRef(2).inputLink()){ // link to the a end output of an operation in the false branch
                usedNodes.insert(item->inputRef(2).inputLink()->id());
            }
        }else if ( item->type() == WorkFlowNode::ntCONDITION){
            // test have operations which have parameters that might be linked to the rest of the graph
            auto subnodes = item->subnodes("tests");
            for(auto subnode : subnodes){
                for(int i=0; i < subnode->inputCount(); ++i){
                    if ( subnode->inputRef(i).inputLink()){
                        usedNodes.insert(subnode->inputRef(i).inputLink()->id());
                    }
                }
            }
        }else if ( item->type() == WorkFlowNode::ntRANGE){
            // test have operations which have parameters that might be linked to the rest of the graph
            auto subnodes = item->subnodes("operations");

            auto parm = item->input(0);
            if ( parm.inputLink()){
                subnodes.push_back(item);
            }
            for(auto subnode : subnodes){
                for(int i=0; i < subnode->inputCount(); ++i){
                    if ( subnode->inputRef(i).inputLink()){
                        usedNodes.insert(subnode->inputRef(i).inputLink()->id());
                    }
                }
            }
            subnodes = item->subnodes("operations");
            for(auto subnode : subnodes){
                if ( subnode->inputRef(RangeNode::rpFINALOUTPUT).inputLink()){
                    usedNodes.insert(subnode->inputRef(RangeNode::rpFINALOUTPUT).inputLink()->id());
                }
            }

        }
    }
    //if a node is not used as input somewhere it must be an output
    std::vector<SPWorkFlowNode> nodes;
    for(auto node : graph){
        if ( node->type() != WorkFlowNode::ntOPERATION) // only operationnodes can create outputs
            continue;
        auto iter = usedNodes.find(node->id());
        if (iter == usedNodes.end()){
            nodes.push_back(node);
        }
    }

    return nodes;
}

std::vector<SPWorkFlowNode> Workflow::outputNodes()
{
    return outputNodes(_graph, this);
}

void Workflow::setFixedParameter(const QString &data, NodeId nodeId, qint32 parmIndex)
{
    SPWorkFlowNode node = nodeById(nodeId);
    if (node){
        if (  parmIndex < node->inputCount()){
            WorkFlowParameter& param = node->inputRef(parmIndex);
            IlwisTypes tp = OperationHelper::determineType(data);
            param.value(data, tp);
            changed(true);
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
            changed(true);
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
        changed(true);
    }
}


void Workflow::removeFlow(NodeId toNode, qint32 parameterIndex)
{
    SPWorkFlowNode node = nodeById(toNode);
    if (node){
        WorkFlowParameter& param = node->inputRef(parameterIndex);
        param.inputLink(SPWorkFlowNode());
        changed(true);
    }
}

void Workflow::addJunctionFlow(int junctionIdTo, const QString &operationIdFrom, int paramIndex, int rectFrom, int rectTo, bool truecase)
{
    SPWorkFlowNode node = nodeById(junctionIdTo);
    SPWorkFlowNode operationFrom = nodeById(operationIdFrom.toULongLong());
    std::shared_ptr<JunctionNode> junction = std::static_pointer_cast<JunctionNode>(node);
    if ( truecase){

        junction->link2trueCase(operationFrom, paramIndex);
        junction->inputRef(1).attachement(rectFrom, true);
        junction->inputRef(1).attachement(rectTo, false);
    }else{
        junction->link2falseCase(operationFrom, paramIndex);
        junction->inputRef(2).attachement(rectFrom, true);
        junction->inputRef(2).attachement(rectTo, false);
    }
    changed(true);

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

void Workflow::translation(double x, double y, bool relative)
{
    if ( relative)
        _translation = std::pair<int,int>(_translation.first + x, _translation.second + y);
    else
        _translation = std::pair<int, int>(x,y);
}

quint32 Workflow::generateId()
{
    return ++_idCounter;
}

void Workflow::updateIdCounter()
{
    qint64 maxid = 0;

    for(SPWorkFlowNode node : _graph){
        maxid = Ilwis::max(maxid, (qint64)node->id());
        auto subnodes = node->subnodes("all");
        for(SPWorkFlowNode subnode : subnodes)
            maxid = Ilwis::max(maxid, (qint64)subnode->id());
    }
    _idCounter = maxid + 1;
}

const std::vector<SPWorkFlowNode> &Workflow::graph() const
{
    return _graph;
}

const std::vector<SPWorkFlowNode> Workflow::nodes(int filter) const{
    std::vector<SPWorkFlowNode> result;
    for(SPWorkFlowNode node : _graph){
        if ( hasType(node->type(), filter)){
            result.push_back(node);
        }
        std::vector<SPWorkFlowNode> subnodes = node->subnodes("all");
        for(auto subnode : subnodes){
            if ( hasType(subnode->type(), filter)){
                result.push_back(subnode);
            }
        }
    }
    return result;
}

bool Workflow::isValid() const
{
    bool ok = true;
    for(const auto& node : _graph){
        ok &= node->isValid(this,WorkFlowNode::vcAGGREGATE);
    }
    return ok;
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
    auto CheckLinks =[&](WorkFlowParameter& p,std::map<NodeId, std::vector<SPOperationParameter>>& outparams)->void {
        // if an outputParameterIndex is defined it can be scrapped from the list of potential free parameters
        if ( p.outputParameterIndex() != iUNDEF){
            // junctionnodes are not regular operations and can be skipped; they are always intermediaries
            if ( p.inputLink()->type() != WorkFlowNode::ntJUNCTION)
                if ( outparams.find(p.inputLink()->id()) != outparams.end()){
                    std::vector<SPOperationParameter>& parms = outparams[p.inputLink()->id()];
                    if (p.outputParameterIndex() <  parms.size())
                        outparams[p.inputLink()->id()][p.outputParameterIndex()] = SPOperationParameter();
                    else
                        throw ErrorObject(TR("Corrupt workflow:") + name());
                }
        }
    };
    std::vector<SPOperationParameter> result;
    std::map<NodeId, std::vector<SPOperationParameter>> outparams;

    for(const auto& item : _graph){
        if (item->type() == WorkFlowNode::ntOPERATION){
            IOperationMetaData md = item->operation();
            if ( md.isValid()){
                outparams[item->id()] = md->getOutputParameters();
            }
        }
        if (hasType(item->type(), WorkFlowNode::ntRANGE)){
            auto subnodes = item->subnodes("operations");
            for(auto subnode : subnodes){
                if (subnode->type() == WorkFlowNode::ntOPERATION){
                    IOperationMetaData md = subnode->operation();
                    if ( md.isValid()){
                        outparams[subnode->id()] = md->getOutputParameters();
                    }
                }
            }
        }
    }
    //we have collected a list of all outputs and are now going to check which ones are linked
    // linked to a previous input and thus not being free; they are scrapped from the list by
    // making its value invalied
    for(const auto& item : _graph){
        if (hasType(item->type(), WorkFlowNode::ntCONDITION)){
            auto subnodes = item->subnodes("tests");
            for(auto subnode : subnodes){
                for(int j=0; j < subnode->inputCount(); ++j){
                    WorkFlowParameter& p = subnode->inputRef(j);
                    CheckLinks(p, outparams);
                }
            }
        }
        if (hasType(item->type(), WorkFlowNode::ntRANGE)){
            auto subnodes = item->subnodes("operations");
            for(auto subnode : subnodes){
                for(int j=0; j < subnode->inputCount(); ++j){
                    WorkFlowParameter& p = subnode->inputRef(j);
                    CheckLinks(p, outparams);
                }
            }
        }

        if ( item->type() == WorkFlowNode::ntJUNCTION){
             //note that the true case (2 == false) isnt under consideration as
            // its output always goes to a junction so it will never appear in the outparams
            // outparams are build on 'visible'nodes and the inside of a condition is not visibile
            // to the outside and the true value comes from there
             WorkFlowParameter& p = item->inputRef(2);
             CheckLinks(p, outparams);

        }else if ( item->type() == WorkFlowNode::ntOPERATION){
            for(int i=0; i < item->inputCount(); ++i){
                WorkFlowParameter& p = item->inputRef(i);
                CheckLinks(p, outparams);
           }
        }

    }
    //collect now all parameters which have not been invalidated; these are the output parameters that are free and must be entered from the outside
    for(auto item : outparams){
        for(auto parm : item.second) {
            if ( parm && parm->linkedInput() == -1)    {
                result.push_back(parm);
            }
        }
    }
    return result;
}

// workflows that are used as operation get rid of the history in their inputnames ( indexes in the internal structure)

void Workflow::reworkInputNames(SPWorkFlowNode& node){
    for(int i=0; i < node->inputCount(); ++i){
        WorkFlowParameter& parm = node->inputRef(i);
        int colonIndex = parm.name().indexOf(":");
        if ( colonIndex != -1){
            if ( colonIndex < 4){
                int spaceIndex = parm.name().indexOf(" ");
                if ( spaceIndex != -1 && spaceIndex < 8){
                    QString newName = parm.name().mid(spaceIndex + 1);
                    parm.name(newName);
                    changed(true);
                }
            }
        }
    }
}

NodeId Workflow::addNode(SPWorkFlowNode node, NodeId parent)
{
    auto iter = std::find(_graph.begin(), _graph.end(), node);
    if ( iter == _graph.end() ){
        if ( node->id() == i64UNDEF)
            node->nodeId(generateId());
        if ( node->type() == WorkFlowNode::ntOPERATION && node->isWorkflow())
            reworkInputNames(node);
        if ( parent != i64UNDEF){
            SPWorkFlowNode parentNode = nodeById(parent);
            if ( parentNode){
                parentNode->addSubNode(node, "operations");
            }
        }else
           _graph.push_back(node);
        changed(true);
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
        if ( hasType(node->type(), WorkFlowNode::ntCONDITION|WorkFlowNode::ntRANGE)){
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
        if ( hasType(node->type(), WorkFlowNode::ntCONDITION|WorkFlowNode::ntRANGE)){
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
    auto removeLinks = [&](NodeId deleteNodeId)->void {
        for(auto linkedNode : _graph){
            int n = linkedNode->type() == WorkFlowNode::ntJUNCTION ? 3 :linkedNode->inputCount() ;
            if ( n > 0){
                for(int i=0; i < n; ++i)    {
                    WorkFlowParameter& param = linkedNode->inputRef(i);
                    if ( param.inputLink()){
                        if ( param.inputLink()->id() == deleteNodeId){
                            param.inputLink(SPWorkFlowNode());
                            changed(true);
                        }
                    }
                }
            }
        }
    };

    for(auto iter = _graph.begin(); iter != _graph.end(); ++iter){
        SPWorkFlowNode node = (*iter);
        if ( node->id() == id){
            std::vector<SPWorkFlowNode> subnodes = node->subnodes("all");
            removeLinks(id);

            for(auto subnode : subnodes)
                removeLinks(subnode->id());
            _graph.erase(iter);
            break;
        }
    }
}

std::map<quint64, int> Workflow::parmid2order() const {
    return _parmid2order;
}

quint64 Workflow::createMetadata(int offset){
    try{
        QString opname = name();
        OperationResource operation = resource();
        operation.addProperty("namespace","ilwis");
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
            _parmid2order[parm.id()] = count + offset;
            QString label = QString("%1 %2").arg(parm.nodeId()).arg(parm.label());
            operation.addInParameter(count,parm.valueType(), label,parm.description(),OperationResource::ueNONE, parm.needsQuotes());
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
    }catch(const ErrorObject& err){

    }
    return i64UNDEF;
}

quint64 Workflow::createMetadata()
{
    return createMetadata(0);
}

IlwisTypes Workflow::ilwisType() const
{
    return itWORKFLOW;
}


//-------------------------------------------------------------------------
WorkflowIdMapping::WorkflowIdMapping(const OperationExpression &expr, const std::map<quint64, int> &mapping) :
    _expression(expr),
    _mapping(mapping)
{

}

QVariant WorkflowIdMapping::getValue(WorkFlowParameter &parm, const ExecutionNode &exnode) const
{
    auto iter = _mapping.find(parm.id());
    if  ( iter != _mapping.end())       {
        int idx = (*iter).second;
        return _expression.parm(idx).value();
    }
    return exnode.parameterValue(parm.order());
}

void WorkflowIdMapping::advanceOffset(int n)
{
    _offset += n;
    _offset = std::max(0,_offset);
}

int WorkflowIdMapping::offset() const
{
    return _offset;
}
