#ifndef EXPRESSIONNODE_H
#define EXPRESSIONNODE_H

namespace Ilwis{

class ExpressionNode : public OperationNode
{
public:
    ExpressionNode();
    QString nodeType() const;
    bool evaluate(SymbolTable& symbols, int scope, ExecutionContext *ctx);

private:
    bool handleAnd(int index,const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
    bool handleOr(int index,const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
    bool handleXor(int index,const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);

};
}

#endif // EXPRESSIONNODE_H
