#ifndef EXPRESSIONNODE_H
#define EXPRESSIONNODE_H

namespace Ilwis{

class ExpressionNode : public OperationNode
{
public:
    ExpressionNode();
    QString nodeType() const;
    bool evaluate(SymbolTable& symbols, int scope);

private:
    bool handleAnd(const NodeValue &vright);
    bool handleOr(const NodeValue &vright);
    bool handleXor(const NodeValue &vright);

};
}

#endif // EXPRESSIONNODE_H
