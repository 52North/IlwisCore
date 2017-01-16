#include "kernel.h"
#include "workflowparameter.h"

using namespace Ilwis;

quint64 WorkFlowParameter::_baseParmId = 0;

WorkFlowParameter::WorkFlowParameter() : _attachements({iUNDEF, iUNDEF})
{

}

WorkFlowParameter::WorkFlowParameter(int order, int nodeid, const QString &name, const QString &description) :
    Identity(name, _baseParmId++, description), _order(order), _nodeid(nodeid), _attachements({iUNDEF, iUNDEF})
{
}

QString WorkFlowParameter::label() const
{
    if ( _label != "")
        return _label;
    return QString::number(_order) + ": " + name();
}

void WorkFlowParameter::label(const QString &lbl)
{
    _label = lbl;
}

void WorkFlowParameter::inputLink(const std::shared_ptr<WorkFlowNode> link, qint32 outputIndex)
{

    _link = link;
    _outputParameterIndex = outputIndex;
    if ( !_link){
        _state = pkFREE;
        _outputParameterIndex = iUNDEF;
        _value = sUNDEF;
   }else
        _state = pkCALCULATED;
}

const std::shared_ptr<WorkFlowNode> WorkFlowParameter::inputLink() const
{
    return _link;
}

std::shared_ptr<WorkFlowNode> WorkFlowParameter::inputLink()
{
    return _link;
}

qint32 WorkFlowParameter::outputParameterIndex() const
{
    return _outputParameterIndex;
}

QString WorkFlowParameter::value() const
{
    return _value;
}

void WorkFlowParameter::value(const QString &v, IlwisTypes type)
{
    _value = v;
    _valueType = type;
    if ( _value != sUNDEF && _state !=  pkCALCULATED)
        _state = pkFIXED;
}

IlwisTypes WorkFlowParameter::valueType() const
{
    return _valueType;
}

WorkFlowParameter::ParameterKind WorkFlowParameter::state() const
{
    return _state;
}

QString WorkFlowParameter::syntax() const
{
    return _syntax;
}

void WorkFlowParameter::addSyntax(const QString &s)
{
    _syntax= s;
}

int WorkFlowParameter::attachement(bool source) const
{
    return source ? _attachements.first : _attachements.second;
}

void WorkFlowParameter::nodeId(int nid)
{
    _nodeid = nid;
}

int WorkFlowParameter::nodeId() const
{
    return _nodeid;
}

int WorkFlowParameter::order() const
{
    return _order;
}

bool WorkFlowParameter::isValid() const
{
    return _nodeid != iUNDEF;
}

void WorkFlowParameter::attachement(int rectIndex, bool source)
{
    if ( source)
        _attachements.first = rectIndex;
    else
        _attachements.second = rectIndex;
}


