#ifndef VARIABLENPDE_H
#define VARIABLENPDE_H

#include <QVariant>
#include "ilwis.h"

namespace Ilwis {
class VariableNode  : public IDNode
{
public:
    VariableNode(bool constant);
    QString nodeType() const;
    void setExpression(ExpressionNode *node);
    bool evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx);

private:
    QSharedPointer<ExpressionNode> _expression;
    bool _constant;
    QVariant _value;

};
}

#endif // VARIABLENPDE_H
