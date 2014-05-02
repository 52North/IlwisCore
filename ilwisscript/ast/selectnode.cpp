#include "kernel.h"
#include "errorobject.h"
#include "astnode.h"
#include "symboltable.h"
#include "geos/geom/Coordinate.h"
#include "geos/geom/Geometry.h"
#include "coordinate.h"
#include "operationnode.h"
#include "expressionnode.h"
#include "ilwisdata.h"
#include "coverage.h"
#include "feature.h"
#include "featurecoverage.h"
#include "selectnode.h"

using namespace Ilwis;

SelectNode::SelectNode()
{
}

QUrl SelectNode::inputUrl() const
{
    return _inputUrl;
}

void SelectNode::setInput(const QString &input)
{
    if ( input.indexOf("url|") == 0 )
        _inputUrl = input.mid(4);
    else
        _inputId = input.mid(3);
}
QString SelectNode::inputId() const
{
    return _inputId;
}

bool SelectNode::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx)
{
    ctx->_additionalInfo["extra0"] = _inputId;
    ctx->_additionalInfo["extra1"] = "true";
    ctx->_useAdditionalParameters = true;
    _expression->evaluate(symbols, scope, ctx);
    return false;
}

void SelectNode::setExpression(ExpressionNode *n)
{
    _expression = QSharedPointer<ExpressionNode>(n);
}





