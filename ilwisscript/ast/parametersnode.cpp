#include <set>
#include "ilwis.h"
#include "identity.h"
#include "astnode.h"
#include "operationnodescript.h"
#include "symboltable.h"
#include "expressionnode.h"
#include "parametersnode.h"

using namespace Ilwis;

ParametersNode::ParametersNode()
{
}

QString ParametersNode::nodeType() const
{
    return "actualParameters";
}

bool ParametersNode::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx)
{
    QList<QVariant> values;
    foreach(QSharedPointer<ASTNode> node, _childeren) {
        if (!node->evaluate(symbols, scope, ctx)) {
            return false;
        } else {
            QVariant var;
            var.setValue(node->value());
            values.push_back(var);
        }
    }
    _value = { values, NodeValue::ctLIST};
    return true;
}
