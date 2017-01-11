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

using namespace Ilwis;

bool Ilwis::operator==(const Ilwis::SPWorkFlowNode& node1, const Ilwis::SPWorkFlowNode& node2 ){
    return node1->id() == node2->id();
}

bool Ilwis::operator<(const Ilwis::SPWorkFlowNode& node1, const Ilwis::SPWorkFlowNode& node2 ){
    return node1->id() < node2->id();
}

WorkFlowNode::WorkFlowNode(const QString& name, const QString& description, quint64 id) : Identity(name, id, description)
{

}

int WorkFlowNode::inputCount() const
{
    return _inputParameters1.size();
}

WorkFlowParameter &WorkFlowNode::inputRef(int index)
{
    if ( index < _inputParameters1.size())
        return _inputParameters1[index];
    throw ErrorObject(TR("Parameter index out of range"));
}

void WorkFlowNode::addInput(const WorkFlowParameter &param, int index)
{
    if ( index == iUNDEF)    {
        _inputParameters1.push_back(param);
    }else{
        if ( index >= _inputParameters1.size())
            _inputParameters1.resize(index+1);
        _inputParameters1[index] = param;
    }
}

void WorkFlowNode::removedInput(int index)
{
    if ( index < _inputParameters1.size())    {
        _inputParameters1.erase(_inputParameters1.begin() + index);
    }
}

void WorkFlowNode::nodeId(quint64 id)
{
    Identity::setId(id);
    for(WorkFlowParameter& param : _inputParameters1){
        param.nodeId(id);
    }
}


void WorkFlowNode::box(const BoundingBox &box)
{
    _box = box;
}

BoundingBox WorkFlowNode::box() const
{
    return _box;
}

bool WorkFlowNode::collapsed() const
{
    return _collapsed;
}

void WorkFlowNode::collapsed(bool yesno)
{
    _collapsed = yesno;
}

std::shared_ptr<WorkFlowNode> WorkFlowNode::owner() const
{
    return _owner;
}

void WorkFlowNode::owner(std::shared_ptr<WorkFlowNode> own)
{
    _owner = own;
}

IOperationMetaData WorkFlowNode::operation() const
{
    return IOperationMetaData();
}

std::vector<SPWorkFlowNode> WorkFlowNode::subnodes(const QString &reason) const
{
    return std::vector<SPWorkFlowNode>();
}

void WorkFlowNode::addSubNode(const SPWorkFlowNode &node, const QString &reason)
{

}

bool WorkFlowNode::execute(ExecutionContext *ctx, SymbolTable &symTable, const OperationExpression &expression, const std::map<quint64, int> &idmap)
{
    return false;
}




