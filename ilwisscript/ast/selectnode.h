#ifndef SELECTNODE_H
#define SELECTNODE_H

namespace Ilwis {
class SelectNode : public ASTNode
{
public:
    SelectNode();

    void setInput(const QString &inputUrl);

    QString inputId() const;

    bool evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx);
    void setExpression(ExpressionNode *n);
private:
    QString _inputId = sUNDEF;
    QSharedPointer<ExpressionNode> _expression;
    bool _isUrl=false;
};
}

typedef std::vector<std::vector<Ilwis::Coordinate>> CoordinateLists;
Q_DECLARE_METATYPE(CoordinateLists);

#endif // SELECTNODE_H
