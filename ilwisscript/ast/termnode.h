#ifndef TERMNODE_H
#define TERMNODE_H

namespace Ilwis {
class TermNode : public ASTNode
{
public:
    TermNode();
    void setNumerical(char *num);
    void setId(IDNode *node);
    void setExpression(ExpressionNode *n);
    void setString(char *s);
    void setParameters(ParametersNode *n);
    void setParameters();
    QString nodeType() const;
    void setLogicalNegation(bool yesno);
    void setNumericalNegation(bool yesno);
    bool evaluate(SymbolTable& symbols, int scope, ExecutionContext *ctx);
    void addSelector(Selector *n);

private:
    enum ContentState{csNumerical, csString, csExpression, csMethod,csID};
    double _number;
    QString _string;
    QSharedPointer<ExpressionNode> _expression;
    QSharedPointer<IDNode> _id;
    QSharedPointer<ParametersNode> _parameters;
    std::vector<QSharedPointer<Selector> > _selectors;
    ContentState _content;
    bool _logicalNegation;
    bool  _numericalNegation;

    QString getName(const Ilwis::NodeValue &var) const;
    QString buildBracketSelection(QString &name);
    bool doMethodStatement(SymbolTable &symbols, int scope, ExecutionContext *ctx);
    bool doIDStatement(SymbolTable &symbols, int scope, ExecutionContext *ctx);
    QString buildStatementSelection(ExecutionContext *ctx);
};
}

#endif // TERMNODE_H
