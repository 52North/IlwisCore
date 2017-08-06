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
#include "executionnode.h"
#include "rangetestnode.h"

using namespace Ilwis;

RangeTestNode::RangeTestNode() : WorkFlowNode("")
{

}

Ilwis::WorkFlowNode::NodeTypes RangeTestNode::type() const
{
    return WorkFlowNode::ntRANGETESTNODE;
}

bool RangeTestNode::isValid(const Ilwis::Workflow *workflow, WorkFlowNode::ValidityCheck check) const
{
    return true;
}

void RangeTestNode::setRangeDefinition(const QVariant &val)
{
    IlwisTypes tp = TypeHelper::variant2type(val);
    if ( tp == itSTRING){
        QString definition = val.toString();
        if ( definition.indexOf("..") > 0){
            QStringList parts = definition.split(QRegExp("(..|:)"));
            if ( parts.size() == 2 ||  parts.size() == 3){
                bool ok;
                double rstart = parts[0].toDouble(&ok);
                if ( ok){
                    double rend = parts[1].toDouble(&ok);
                    if ( ok){
                        if ( parts.size() == 3){
                            double prec = parts[2].toDouble(&ok);
                            if ( ok){
                                if ( prec == 0){
                                    throw ErrorObject(TR("Illegal precission value; it can not be 0"));
                                }
                                _precision = prec;
                                _rangeStart = rstart;
                                _rangeEnd = rend;
                                _case = ccLIMITS;
                                _currentValue = rstart;
                            }
                        }else {
                            _rangeStart = rstart;
                            _rangeEnd = rend;
                            _case = ccLIMITS;
                            _currentValue = rstart;
                        }
                    }
                }
            }
        }else {
            QStringList parts = val.toString().split("|");
            std::copy(parts.begin(), parts.end(), std::back_inserter(_rangeValues));
            _case = ccVECTOR;
            _currentIndex = 0;
        }
    }
}

bool RangeTestNode::next()
{
    if ( _case == ccVECTOR)    {
        if ( _currentIndex != rUNDEF && _currentIndex >= _rangeValues.size())
            return false;
        if ( _currentIndex != rUNDEF && _currentIndex <= _rangeValues.size())
            ++_currentIndex;
    }else if ( _case == ccLIMITS){
        if ( _currentValue != rUNDEF && _currentValue >= _rangeEnd)
            return false;
        if ( _currentValue != rUNDEF && _currentValue <= _rangeEnd)
            _currentValue += _precision;
    }
    return true;
}

QVariant RangeTestNode::currentValue() const
{
    if ( _case == ccVECTOR)    {
        if ( _currentIndex != rUNDEF && _currentIndex >= _rangeValues.size())
            return QVariant();
        if ( _currentIndex != rUNDEF && _currentIndex <= _rangeValues.size())
            return _rangeValues[_currentIndex];
    }else if ( _case == ccLIMITS){
        if ( _currentValue != rUNDEF && _currentValue >= _rangeEnd)
            return QVariant();
        if ( _currentValue != rUNDEF && _currentValue <= _rangeEnd)
            return _currentValue;
    }
    return QVariant();
}
