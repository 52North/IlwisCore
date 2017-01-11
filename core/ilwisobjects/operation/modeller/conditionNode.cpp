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

QString WorkFlowCondition::type() const
{
    return "conditionnode"    ;
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
            parts = parts[1].split("|");
            NodeId linkedNodeId = parts[0].toULongLong();
            int outParmIndex = parts[1].toInt();
            SPWorkFlowNode linkNode = wf->nodeById(linkedNodeId);
            test._operation->inputRef(parameterIndex).inputLink(linkNode,outParmIndex);
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


bool WorkFlowCondition::execute(ExecutionContext *ctx, SymbolTable &symTable, const OperationExpression &expression, const std::map<quint64, int> &idmap)
{
    bool testRestult = true;
    for(Test& test : _tests){
       SymbolTable symTableLocal(symTable);
       ExecutionContext ctx;
       if (!test._operation->execute(&ctx,symTable,expression, idmap))
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

void WorkFlowCondition::nodeId(quint64 id)
{
    WorkFlowNode::nodeId(id);
    name(QString("condition_%1").arg(id));
}
