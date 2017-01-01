#include "coverage.h"
#include "errorobject.h"
#include "astnode.h"
#include "symboltable.h"
#include "geos/geom/Coordinate.h"
#include "geos/geom/Geometry.h"
#include "ilwiscoordinate.h"
#include "operationnodescript.h"
#include "expressionnode.h"
#include "featurecoverage.h"
#include "feature.h"
#include "selectnode.h"

using namespace Ilwis;

SelectNode::SelectNode()
{
}

void SelectNode::setInput(const QString &input)
{
    if ( input.indexOf("url|") == 0 ){

        _inputId = "\"" + QString(input).remove("\"") + "\"";
        _isUrl = true;
    }
    else
        _inputId = input.mid(3);
}
QString SelectNode::inputId() const
{
    return _inputId;
}

bool SelectNode::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx)
{
    ctx->_additionalInfo[IMPLICITPARMATER0] = _inputId;
    ctx->_useAdditionalParameters = true;
    if(!_expression->evaluate(symbols, scope, ctx))
        return false;

    _value = _expression->value();

    return true;
}

void SelectNode::setExpression(ExpressionNode *n)
{
    _expression = QSharedPointer<ExpressionNode>(n);
}





