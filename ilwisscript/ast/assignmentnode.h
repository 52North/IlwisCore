#ifndef AssignmentNode_H
#define AssignmentNode_H

namespace Ilwis {
class AssignmentNode : public ASTNode
{
public:
    AssignmentNode();
    bool isDefinition() const;
    void setDefintion(bool yesno);
    void setResult(IDNode *node);
    //void setDomPart(ASTNode *node);
    void setExpression(ExpressionNode *node);
    QString nodeType() const;
    bool evaluate(SymbolTable &symbols, int scope);
    void setFormatPart(ASTNode *node);
private:
    bool _defintion;
    QSharedPointer<IDNode> _result;
    QSharedPointer<ASTNode> _typemodifier;
    QSharedPointer<ExpressionNode> _expression;
 };
}

#endif // AssignmentNode_H
