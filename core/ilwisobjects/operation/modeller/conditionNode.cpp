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
#include "workflownode.h"
#include "workflow.h"
#include "mastercatalog.h"
#include "operationhelper.h"
#include "conditionNode.h"

using namespace Ilwis;

WorkFlowCondition::WorkFlowCondition() : WorkFlowNode("")
{

}

WorkFlowNode::NodeTypes WorkFlowCondition::type() const
{
    return WorkFlowNode::ntCONDITION;
}

std::vector<SPWorkFlowNode> WorkFlowCondition::subnodes(const QString &reason) const
{
    std::vector<SPWorkFlowNode> all;

    if ( reason == "operations")
        return _operations;

    for(auto &test : _tests){
        all.push_back(test._operation);
    }
    if ( reason == "tests"){
        return all;
    }
    std::copy(_operations.begin(), _operations.end(), std::back_inserter(all));

    return all;

}

void WorkFlowCondition::addSubNode(const SPWorkFlowNode &node, const QString &reason)
{
    if (reason == "operations"){
        if ( node->id() == i64UNDEF){
            kernel()->issues()->log(TR("Attempt to add invalid node to workflow; it has no id "));
            return;
        }
        _operations.push_back(node);
    }
}

void WorkFlowCondition::addTest(SPWorkFlowNode node,LogicalOperator pre, LogicalOperator post){
    Test test;
    if ( pre != loNONE && pre != loNOT)
        kernel()->issues()->log(TR("Illegal construct in test of a condtion:") + QString::number(node->id()));
    if ( _tests.size() == 0 && post != loNONE)
        kernel()->issues()->log(TR("Illegal construct in test of a condtion:") + QString::number(node->id()));
    if ( _tests.size() > 0 && !( post == loAND || post == loOR))
         kernel()->issues()->log(TR("Illegal construct in test of a condtion:") + QString::number(node->id()));
    test._pre = pre;
    test._post = post;
    test._operation = node;
    _tests.push_back(test);

}

void WorkFlowCondition::setTestValue(int testIndex, int parameterIndex, const QString &value, const IWorkflow &wf)
{
    if ( testIndex < _tests.size()) {
        Test& test = _tests[testIndex];
        if ( value.indexOf("link=") != 0){ // case were the value of the test is not a link to another node
            auto parms = test._operation->operation()->getInputParameters();
            if ( parameterIndex < parms.size()){
                SPOperationParameter parm = parms[parameterIndex];
                test._operation->inputRef(parameterIndex).value(value, parm->type());
            }
        }else {
            QStringList parts = value.split("=");
            if ( parts.size() == 2){
                parts = parts[1].split(":");
                if (parts.size() == 2) {
                    NodeId linkedNodeId = parts[0].toULongLong();
                    int outParmIndex = parts[1].toInt();
                    SPWorkFlowNode linkNode = wf->nodeById(linkedNodeId);
                    test._operation->inputRef(parameterIndex).inputLink(linkNode,outParmIndex);
                }
            }
        }
    }
}

QString WorkFlowCondition::testValue(int testIndex, int parameterIndex, const IWorkflow &wf) const
{
    if ( testIndex < _tests.size()) {
        Test test = _tests[testIndex];
        return test._operation->inputRef(parameterIndex).value();
    }
    return sUNDEF;
}

WorkFlowCondition::Test WorkFlowCondition::test(int index) const
{
    if ( index < _tests.size()){
        return _tests[index];
    }
    return Test();
}

int WorkFlowCondition::testCount() const
{
    return _tests.size();
}

void WorkFlowCondition::nodeId(quint64 id)
{
    WorkFlowNode::nodeId(id);
    name(QString("condition_%1").arg(id));
}

bool WorkFlowCondition::isValid(const Workflow *workflow, WorkFlowNode::ValidityCheck check) const
{
    bool testsOk = _tests.size() > 0;
    for(Test test : _tests){
        testsOk &= test.isValid();
    }
    if ( check == vcTESTS)
        return testsOk;
    // all operations in a condition must have a defined value  ( calculated or fixed) else you get optional parameters which is for the moment not supported
    bool operationsOk = _operations.size() > 0;
    for(SPWorkFlowNode node : _operations){
       if ( !node->isValid(workflow, WorkFlowNode::vcALLDEFINED))
           operationsOk = false;

    }
    if ( check == vcOPERATIONS)
        return operationsOk;
    return testsOk && operationsOk;
}

bool WorkFlowCondition::Test::isValid() const
{
    return _operation && _operation->isValid(0, WorkFlowNode::vcALLDEFINED);
}

void WorkFlowCondition::removeSubNode(NodeId dNodeId)
{
    auto KillNode2 = [](std::vector<SPWorkFlowNode>& nodes, NodeId deletedNodeId) ->void{
        for(SPWorkFlowNode node : nodes){
            for(int i=0; i < node->inputCount(); ++i){
                WorkFlowParameter& param = node->inputRef(i);
                if ( param.inputLink()){
                    if ( param.inputLink()->id() == deletedNodeId){
                        param.inputLink(SPWorkFlowNode());
                    }
                }
            }
        }
    };
    auto KillNode1 = [&](std::vector<SPWorkFlowNode>& nodes, NodeId dNodeId){
        auto iter = nodes.begin();
        for(; iter != nodes.end(); ++iter){
            if ( (*iter)->id() == dNodeId)
                break;
        }
        if ( iter != nodes.end()){
            SPWorkFlowNode dNode = (*iter);
            nodes.erase(iter);
            KillNode2(nodes,dNodeId );
        }
    };
    KillNode1(_operations, dNodeId);
}
