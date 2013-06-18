#ifndef WHILENODE_H
#define WHILENODE_H

namespace Ilwis {
class WhileNode : public ASTNode
{
public:
    WhileNode();
    void setCondition(ExpressionNode *expr);
    QString nodeType() const;


private:
    QSharedPointer<ExpressionNode> _condition;

};
}

#endif // WHILENODE_H
