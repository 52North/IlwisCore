#ifndef IFNODE_H
#define IFNODE_H

namespace Ilwis {


class Ifnode : public ASTNode
{
public:
    Ifnode();
    bool evaluate(SymbolTable& symbols, int scope, ExecutionContext *ctx);
    void setCondition(ExpressionNode *expr);
    void addThen(ASTNode *node);
    void addElse(ASTNode *node);

private:
    QSharedPointer<ExpressionNode> _condition;
    std::vector<QSharedPointer<ASTNode>> _then;
    std::vector<QSharedPointer<ASTNode>> _else;



};
}

#endif // IFNODE_H
