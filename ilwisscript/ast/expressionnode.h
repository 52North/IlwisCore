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
    bool handleAnd(const NodeValue &vright, Ilwis::SymbolTable &symbols);
    bool handleOr(const NodeValue &vright, Ilwis::SymbolTable &symbols);
    bool handleXor(const NodeValue &vright, Ilwis::SymbolTable &symbols);

};
}

#endif // EXPRESSIONNODE_H
