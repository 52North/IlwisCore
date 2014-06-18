#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "logicalexpressionparser.h"

using namespace Ilwis;

LogicalExpressionParser::LogicalExpressionParser()
{
}

LogicalExpressionParser::LogicalExpressionParser(const QString &expr)
{
    parse(expr);
}

bool LogicalExpressionParser::parse(const QString &ex)
{
    int index = 0;
    QString expr = ex.toLower();
    LogicalOperator operLast = loNONE;
    while(index != -1) {
        int shift = 0;
        LogicalOperator oper = loNONE;
        int newindex = expr.indexOf(" and ", index);
        if (newindex == -1){
            newindex = expr.indexOf(" or ", index);
            if ( newindex != -1){
                oper = loOR;
                shift = 3;
            }
        }else{
            oper = loAND;
            shift = 4;
        }
        LogicalExpressionPart part(ex.mid(index, newindex != -1 ? newindex : ex.size()));
        if ( !part.isValid()){
            _parts.clear();
            return ERROR2(ERR_ILLEGAL_VALUE_2,TR("expression"), ex);
        }
        part.logicalConnector(operLast);
        _parts.push_back(part);
        index = newindex + shift;
        operLast = oper;
    }
    return true;
}

bool LogicalExpressionParser::isValid() const
{
    return _parts.size() > 0;
}

std::vector<LogicalExpressionPart> LogicalExpressionParser::parts() const
{
    return _parts;
}

//---------------------------------------------------
LogicalExpressionPart::LogicalExpressionPart(const QString &ex)
{
    QString expr = ex.trimmed();
    typedef std::pair<QString, LogicalOperator> CondP;
    std::map<QString, LogicalOperator> conditions = {CondP("<=", loLESSEQ),CondP(">=", loGREATEREQ),
                                                     CondP("!=", loNEQ),CondP("==", loEQ),
                                                     CondP("<", loLESS),CondP(">", loGREATER)};
    for(auto condition : conditions) {
        int index = -1;
        if ( (index = expr.indexOf(condition.first)) != -1){
            QString left = expr.left(index);
            QString right = expr.mid(index + condition.first.size());
            _field = left.trimmed();
            _value = right.trimmed();
            _value = _value.remove('\"');
            _condition = condition.second;
            _vt = Domain::ilwType(_value);
            if ( _vt == itUNKNOWN)
                _vt = itSTRING;
            break;
        }
    }
}

QString LogicalExpressionPart::field() const
{
    return _field;
}

LogicalOperator LogicalExpressionPart::condition() const
{
    return _condition;
}

QString LogicalExpressionPart::value() const
{
    return _value;
}

IlwisTypes LogicalExpressionPart::valueType() const
{
    return _vt;
}

LogicalOperator LogicalExpressionPart::logicalConnector() const
{
    return _logicalOper;
}

bool LogicalExpressionPart::isValid() const
{
    return _field != sUNDEF && _condition != loNONE && _vt != itUNKNOWN;
}

void LogicalExpressionPart::logicalConnector(LogicalOperator oper)
{
    _logicalOper  = oper;
}
