#include <QStringList>

#include "ilwis.h"
#include "angle.h"
#include "point.h"
#include "box.h"
#include "astnode.h"
#include "idnode.h"
#include "kernel.h"
#include "symboltable.h"
#include "operationnode.h"
#include "expressionnode.h"
#include "parametersnode.h"
#include "rangedefinitionnode.h"
#include "termnode.h"
#include "commandhandler.h"

TermNode::TermNode() : _logicalNegation(false), _numericalNegation(false)
{
}

void TermNode::setNumerical(char *num)
{
    QString s(num);
    _content = csNumerical;
    if ( s == "?") {
        _number = Ilwis::rUNDEF;
        return;
    }
    bool ok;
    double b =s.toDouble(&ok);
    _number = b;
    if(!ok) {
        _number = Ilwis::rUNDEF;
    }

}

void TermNode::setId(IDNode *node)
{
    _id = QSharedPointer<IDNode>(node)   ;
    _content = csID;
}

void TermNode::setExpression(ExpressionNode *n)
{
    _expression = QSharedPointer<ExpressionNode>(n);
    _content = csExpression;
}

void TermNode::setString(char *s)
{
    _string = QString(s);
    _content = csString;
}

void TermNode::setParameters(ParametersNode *n)
{
    _parameters = QSharedPointer<ParametersNode>(n);
    _content = csMethod;
}

QString TermNode::nodeType() const
{
    return "term";
}

void TermNode::setLogicalNegation(bool yesno)
{
    _logicalNegation = yesno;
}


void TermNode::setNumericalNegation(bool yesno)
{
    _numericalNegation = yesno;
}

bool TermNode::evaluate(SymbolTable &symbols, int scope)
{
    if ( _content == csExpression)  {
        if (_expression->evaluate(symbols, scope)) {
            _value = {_expression->value(), NodeValue::ctExpression};
            return true;
        }
    } else if (_content == csNumerical) {
        if ( _numericalNegation)
            _value = {-_number, NodeValue::ctNumerical};
        else
            _value = {_number, NodeValue::ctNumerical};
        return true;

    } else if ( _content == csString) {
        _value = {_string, NodeValue::ctString};

    } else if ( _content == csMethod) {
        QString parms = "(";
        for(int i=0; i < _parameters->noOfChilderen(); ++i) {
            bool ok = _parameters->child(i)->evaluate(symbols, scope);
            if (!ok)
                return false;

            if ( i != 0)
                parms += ",";
            parms += _parameters->child(i)->value().toString();

        }
        parms += ")";
        QString expression = _id->id() + parms;
        Ilwis::ExecutionContext ctx;
        bool ok = Ilwis::commandhandler()->execute(expression, &ctx);
        if ( !ok || ctx._results.size() != 1)
            return false;
        _value = {ctx._results[0], NodeValue::ctMethod};
        return true;

    } else if ( _content == csID) {
        bool ok = true;
        _id->evaluate(symbols, scope);
        QString value;
        if ( _id->isReference())
            value = symbols.get(_id->id(), scope, ok).toString();
        else
            value = _id->id();
        if ( _ranges.size() > 0) {
            for(QSharedPointer<RangeDefinitionNode> range: _ranges)  {
                if ( range->box().isValid())
                    value += "[" + range->box().toString() + "]";
                else
                    value += "[" + range->variable() + "]";
            }
        }
        _value = {value, NodeValue::ctID};
        return ok;

    }
    return false;
}

void TermNode::addRange(RangeDefinitionNode *n)
{
    _ranges.push_back(QSharedPointer<RangeDefinitionNode>(n));
}
