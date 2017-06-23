#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
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
#include "pythonworkflowconnector.h"
#include "generatescriptnode.h"

using namespace Ilwis;
using namespace PythonScript;

GenerateScriptNode::GenerateScriptNode(){

}

GenerateScriptNode::GenerateScriptNode(const SPWorkFlowNode& node) : _node(node)
{
    _parameterValues.resize(node->inputCount(),sUNDEF);
}

std::vector<QString> GenerateScriptNode::execute(const OperationExpression &expression, const std::map<quint64, int> &idmap, QStringList &script, const QString& whitespace)
{
    switch(_node->type()) {
    case WorkFlowNode::ntCONDITION:
        return executeCondition(expression, idmap, script);
    case WorkFlowNode::ntJUNCTION:
        return executeJunction( expression, idmap, script);
    case WorkFlowNode::ntOPERATION:
        return executeOperation(expression, idmap, script,whitespace);
    case WorkFlowNode::ntRANGE:
        return executeLoop(expression, idmap, script);
    default:
        return std::vector<QString>();
    }
    return std::vector<QString>();
}

std::vector<QString> GenerateScriptNode::executeOperation(const OperationExpression &expression, const std::map<quint64, int> &idmap, QStringList& script, const QString& whitespace)
{
    auto getValue = [&](const WorkFlowParameter& parm, const OperationExpression& expression, const std::map<quint64, int>& idmap)-> QVariant{
        auto iter = idmap.find(parm.id());
        if  ( iter != idmap.end())       {
            int idx = (*iter).second;
            QString value =  expression.parm(idx).value();
            if ( hasType(parm.valueType(), itILWISOBJECT)){
                if ( value.indexOf("?input") != 0){
                    if ( value.indexOf("://") != -1){
                        value = "'"     + value + "'";
                    }
                }
            }
            return value;
        }
        return parameterValue(parm.order());
    };
    int inputCount = _node->inputCount();
    for(int i=0; i < inputCount; ++i){
        WorkFlowParameter& parameter = _node->inputRef(i);
        if ( parameterValue(i) == sUNDEF){
            if (parameter.inputLink()) {
                GenerateScriptNode exNode(parameter.inputLink());
                auto outs = exNode.execute(expression, idmap, script,whitespace);
                if ( parameter.outputParameterIndex() < outs.size())
                    _parameterValues[i] = outs[parameter.outputParameterIndex()];
            }
        }
    }

    SPOperationNode opNode = std::static_pointer_cast<OperationNode>(_node);
    IOperationMetaData metadata = opNode->operation();
    QString expr = "ilwis.Engine.do('"+ metadata->name()  + "',";
    QString parms;
    for(int i=0; i < inputCount; ++i){
        WorkFlowParameter& inParam = _node->inputRef(i);
        if ( parms != "")
            parms += ",";
        if ( hasType(inParam.valueType(),itILWISOBJECT)){
            parms += getValue(inParam, expression, idmap).toString();
        }else if ( hasType(inParam.valueType(), itINTEGER | itFLOAT | itDOUBLE )) {
            QString val = getValue(inParam,expression, idmap).toString();
            bool ok;
            val.toDouble(&ok); // some parms can be strings or numbers; we test what it actually is
            if (!ok)
                val = "\'" + val + "\'";
            parms += val;
        } else if (hasType(inParam.valueType(),itSTRING)){
            parms += "\'" + getValue(inParam,expression, idmap).toString() + "\'";
        }else
            parms += getValue(inParam,expression, idmap).toString();
    }
    std::vector<QString> names;
    QString nameslist;
    for(int i=0; i < opNode->operation()->outputParameterCount(); ++i){
        QString name = opNode->operation()->name() + "_" + QString::number(_node->id()) + "_" + QString::number(i);
        names.push_back(name);
        nameslist = nameslist + (i==0 ? "" : ",") + name;
    }
    expr = nameslist + (nameslist!="" ? "=" : "") + expr;
    expr = expr + parms + ")";
    if(!script.contains(expr)){

        script.append(whitespace + expr);
    }

    return names;

}

std::vector<QString> GenerateScriptNode::executeCondition(const OperationExpression &expression, const std::map<quint64, int> &idmap, QStringList& script)
{
    SPCondition condition = std::static_pointer_cast<WorkFlowCondition>(_node);
    QString expr = "if ";
    QStringList otherLines;
    for(int i=0; i < condition->testCount(); ++i){
        const WorkFlowCondition::Test& test = condition->test(i);
        GenerateScriptNode exNode(test._operation);
        QStringList testLines;
        exNode.execute(expression, idmap, testLines);
        for(int i=0; i < testLines.size() - 1; ++i)
            otherLines.push_back(testLines[i]);
        QString testLine = testLines.last();
        // there is an assignment here which we dont need
        int index = testLine.indexOf("=");
        testLine = testLine.mid(index+1);

        if ( test._pre != loNONE){
            if ( test._pre == loNOT)
                testLine += "!" + testLines[0];
        }
        if ( test._post == loAND){
            testLine  += " && " + testLines[0];
        }else if ( test._post == loOR)
            testLine  += " || " + testLines[0];
        expr += testLine;

    }
    expr += ":";
    script.append(otherLines);
    script.append(expr);

    return std::vector<QString>();
}

std::vector<QString> GenerateScriptNode::executeJunction(const OperationExpression &expression, const std::map<quint64, int> &idmap, QStringList& script)
{
    WorkFlowParameter& condParm = _node->inputRef(0);
    if (parameterValue(0) == sUNDEF){
        GenerateScriptNode exNode(condParm.inputLink());
        exNode.execute(expression, idmap, script);
    }
    QString varName = "junction_" + QString::number(_node->id());
    GenerateScriptNode exNode1(_node->inputRef(1).inputLink());
    exNode1.execute(expression, idmap, script,"\t");
    QString lastLine = script.last();
    int index = lastLine.indexOf("=");
    lastLine = "\t"+ varName + "=" + lastLine.mid(index + 1);
    script.last() = lastLine;
    script.append("else:");
    GenerateScriptNode exNode2(_node->inputRef(2).inputLink());
    exNode2.execute(expression, idmap, script,"\t");
    lastLine = script.last();
    index = lastLine.indexOf("=");
    lastLine = "\t"+ varName + "=" + lastLine.mid(index + 1);
    script.last() = lastLine;

    return {varName};
}

std::vector<QString> GenerateScriptNode::executeContent(const OperationExpression &expression, const std::map<quint64, int> &idmap, QStringList& script){
    return std::vector<QString>();
}

bool GenerateScriptNode::next(){
    return false;
}

QString GenerateScriptNode::parameterValue(int parmIndex) const
{
    if ( _node->inputRef(parmIndex).state() == WorkFlowParameter::pkCALCULATED)
        return _parameterValues[parmIndex];
    return _node->inputRef(parmIndex).value();
}

std::vector<QString> GenerateScriptNode::executeLoop(const OperationExpression &expression, const std::map<quint64, int> &idmap, QStringList& script)
{

    return std::vector<QString>();
}

void GenerateScriptNode::clearCalculatedValues(){
    for(QString& v : _parameterValues)
        v = sUNDEF;
}

