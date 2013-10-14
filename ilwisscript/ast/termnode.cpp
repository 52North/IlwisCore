#include <QStringList>

#include "kernel.h"
#include "raster.h"
#include "astnode.h"
#include "idnode.h"
#include "kernel.h"
#include "symboltable.h"
#include "operationnode.h"
#include "expressionnode.h"
#include "parametersnode.h"
#include "selectornode.h"
#include "termnode.h"
#include "commandhandler.h"

using namespace Ilwis;

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

bool TermNode::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx)
{
    if ( _content == csExpression)  {
        if (_expression->evaluate(symbols, scope, ctx)) {
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
        return true;

    } else if ( _content == csMethod) {
        QString parms = "(";
        for(int i=0; i < _parameters->noOfChilderen(); ++i) {
            bool ok = _parameters->child(i)->evaluate(symbols, scope, ctx);
            if (!ok)
                return false;
            QString name = getName(_parameters->child(i)->value());
            if ( i != 0)
                parms += ",";
            parms += name;

        }
        parms += ")";
        QString expression = _id->id() + parms;
        bool ok = Ilwis::commandhandler()->execute(expression, ctx, symbols);
        if ( !ok || ctx->_results.size() != 1)
            throw ScriptExecutionError(TR("Expression execution error in script; script aborted. See log for further details"));

        _value = {symbols.getValue(ctx->_results[0]), ctx->_results[0], NodeValue::ctMethod};
        return true;

    } else if ( _content == csID) {
        _id->evaluate(symbols, scope, ctx);
        QString value;
        if ( _id->isReference()) {
            if ( symbols.getSymbol(_id->id(),scope).isValid())
                value = _id->id();
            else
                return false;
        }
        else
            value = _id->id();
        if ( _selectors.size() > 0) {
            // selectors are handled by successive calls to the selection operation and in the end giving the temp object to the value
            for(QSharedPointer<Selector> selector: _selectors)  {
                QString selectordef;
                if ( !selector->box().isNull())
                    selectordef = QString("\"box=%1 %2, %3 %4\"").arg(selector->box().min_corner().x()).arg(selector->box().min_corner().y()).
                                                                arg(selector->box().max_corner().x()).arg(selector->box().max_corner().y());
                else if (selector->selectorType() == "index") {
                    selectordef = "\"index=" + selector->variable() + "\"";
                } else if (selector->selectorType() == "columnrange") {
                    selectordef = QString("\"columnrange=%1,%2").arg(selector->beginCol()).arg(selector->endCol());
                    if ( selector->keyColumns() != sUNDEF)
                        selectordef += "," + selector->keyColumns();
                    selectordef += "\"";
                }else if (selector->selectorType() == "recordrange") {
                    selectordef = QString("\"recordrange=%1,%2").arg(selector->beginRec()).arg(selector->endRec());
                    selectordef += "\"";
                }else if (selector->selectorType() == "columnrecordrange") {
                    selectordef = QString("\"columnrecordrange=%1,%2,%3, %4").arg(selector->beginCol()).
                                                                              arg(selector->endCol()).
                                                                              arg(selector->beginRec()).
                                                                              arg(selector->endRec());
                    if ( selector->keyColumns() != sUNDEF)
                        selectordef += "," + selector->keyColumns();
                    selectordef += "\"";
                }
                else if ( selector->variable() != sUNDEF)
                    selectordef = "\"attribute=" + selector->variable() + "\"";
                QString outname = ANONYMOUS_PREFIX;
                QString expression = QString("%1=selection(%2,%3)").arg(outname).arg(value).arg(selectordef);
                if(!Ilwis::commandhandler()->execute(expression, ctx, symbols)) {
                    throw ScriptExecutionError(TR("Expression execution error in script; script aborted. See log for further details"));
                }
                QString outgc = ctx->_results[0];
                value = outgc;
            }
        }
        _value = {value, NodeValue::ctID};
        return value != "" && value != sUNDEF;

    }
    return false;
}

QString TermNode::getName(const NodeValue& var) const {
    QString name = var.toString();
    if (name != sUNDEF)
        return name;
//    QString typeName = var.typeName();
//    if ( typeName == "Ilwis::IRasterCoverage") {
//        Ilwis::IRasterCoverage raster = var.value<Ilwis::IRasterCoverage>();
//        name = raster->name();
//    }
//    if ( typeName == "Coordinate") {
//        name = var.id();
//    }
//    if ( typeName == "Voxel") {
//        name = var.id();
//    }
    return var.id();
}

void TermNode::addSelector(Selector *n)
{
    _selectors.push_back(QSharedPointer<Selector>(n));
}
