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
#include "raster.h"
#include "mastercatalog.h"
#include "operationhelper.h"
#include "executionnode.h"
#include "rangenode.h"

using namespace Ilwis;


RangeNode::RangeNode() : WorkFlowNode("")
{
        _inputParameters1.push_back(WorkFlowParameter(0,id(),"rangetest"));
}

Ilwis::WorkFlowNode::NodeTypes Ilwis::RangeNode::type() const
{
    return WorkFlowNode::ntRANGE;
}

bool RangeNode::isValid(const Ilwis::Workflow *workflow, WorkFlowNode::ValidityCheck check) const
{
    return  true;
}

void RangeNode::addSubNode(const std::shared_ptr<WorkFlowNode> &node, const QString &reason)
{
    if (reason == "operations"){
        if ( node->id() == i64UNDEF){
            kernel()->issues()->log(TR("Attempt to add invalid node to workflow; it has no id "));
            return;
        }
        _operations.push_back(node);
    }
    if (reason == "junctions"){
        if ( node->id() == i64UNDEF){
            kernel()->issues()->log(TR("Attempt to add invalid node to workflow; it has no id "));
            return;
        }
        _junctions.push_back(node);
    }
}

void RangeNode::removeSubNode(NodeId dNodeId)
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
    KillNode1(_junctions, dNodeId);
}

std::vector<std::shared_ptr<WorkFlowNode> > RangeNode::subnodes(const QString &reason) const
{
    if ( reason == "operations")
        return _operations;
    if ( reason == "junctions")
        return _junctions;
    std::vector<SPWorkFlowNode > all(_operations.begin(), _operations.end());
    std::copy(_junctions.begin(), _junctions.end(), std::back_inserter(all));

    return all;
}

void RangeNode::nodeId(quint64 id)
{
    WorkFlowNode::nodeId(id);
    name(QString("range_%1").arg(id));
}

int RangeNode::inputCount() const
{
    return 1;
}

void RangeNode::setRangeDefinition(const QString &val, const Workflow* workflow)
{
    _rangeDef = val;
    if ( val.indexOf("link=") == 0){ // we cant know anything about the range, it will be resolved when the link is executed
        _case = ccUNKNOWN;
        _rangeStart = rUNDEF;
        _rangeEnd= rUNDEF;
        _precision = 1;
        _rangeValues = std::vector<QString> ();
        NodeId nid = i64UNDEF;
        int outPIndex = iUNDEF;
        if(!checkLinkDefintion(_rangeDef, nid, outPIndex)){
            kernel()->issues()->log(TR("illegal link defintion in  rangedefinition:") +_rangeDef );
            _rangeDef = "";
            return;
        }
        SPWorkFlowNode node = workflow->nodeById(nid);
        if ( !node){
            kernel()->issues()->log(TR("illegal node in link defintion in  rangedefinition:") +_rangeDef );
            _rangeDef = "";
        }else
            _inputParameters1[0].inputLink(node, outPIndex);
        return;
    }
    if ( _rangeDef.indexOf("..") > 0){
        QStringList parts = _rangeDef.split("..");
        if ( parts.size() == 2){
            bool ok;
            double rstart = parts[0].toDouble(&ok);
            if ( ok){
                QStringList parts2 = parts[1].split(":");
                double rend = parts2[0].toDouble(&ok);
                if ( ok){
                    if ( parts2.size() == 2){
                        double prec = parts2[1].toDouble(&ok);
                        if ( ok){
                            if ( prec == 0){
                                _rangeDef = "";
                                throw ErrorObject(TR("Illegal precission value; it can not be 0"));
                            }
                            _precision = prec;
                            _rangeStart = rstart;
                            _rangeEnd = rend;
                            _case = ccLIMITS;
                            _currentValue = rUNDEF;
                        }
                    }else {
                        _rangeStart = rstart;
                        _rangeEnd = rend;
                        _case = ccLIMITS;
                        _currentValue = rUNDEF;
                    }
                    _inputParameters1[0].value(_rangeDef, itVALUERANGE, WorkFlowParameter::pkFIXED);
                }
            }
        }
    }else {
        _rangeValues = std::vector<QString>();
        QStringList parts = val.split(",");
        std::copy(parts.begin(), parts.end(), std::back_inserter(_rangeValues));
        _case = ccVECTOR;
        _currentIndex = iUNDEF;
        _inputParameters1[0].value(_rangeDef, itNUMERIC|itCOLLECTION, WorkFlowParameter::pkFIXED);
    }
}

QString RangeNode::rangeDefinition() const
{
    return  _rangeDef;
}

bool RangeNode::next()
{
    if ( _case == ccVECTOR)    {
        if ( _currentIndex != iUNDEF && _currentIndex >= _rangeValues.size() - 1)
            return false;
        if ( _currentIndex == iUNDEF || _currentIndex < _rangeValues.size() - 1)
            _currentIndex = (_currentIndex == iUNDEF ? 0 : ++_currentIndex);
    }else if ( _case == ccLIMITS){
        if ( _currentValue != rUNDEF && _currentValue >= _rangeEnd)
            return false;
        if ( _currentValue == rUNDEF || _currentValue < _rangeEnd)
            _currentValue = (_currentValue == rUNDEF ? _rangeStart : _currentValue + _precision);
    } 
    return true;
}

QVariant RangeNode::currentValue() const
{
    if ( _case == ccVECTOR)    {
        if ( _currentIndex == iUNDEF || _currentIndex >= _rangeValues.size())
            return QVariant();
        if ( _currentIndex !=iUNDEF && _currentIndex < _rangeValues.size())
            return _rangeValues[_currentIndex];
    }else if ( _case == ccLIMITS){
        if ( _currentValue != rUNDEF && _currentValue > _rangeEnd)
            return QVariant();
        if ( _currentValue != rUNDEF && _currentValue <= _rangeEnd)
            return _currentValue;
    }
    return QVariant();
}
