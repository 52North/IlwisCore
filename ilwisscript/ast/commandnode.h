#ifndef COMMANDNODE_H
#define COMMANDNODE_H

namespace Ilwis {
class CommandNode : public ASTNode
{
public:
    CommandNode();
    QString nodeType() const;
    void setCommand(const QString& com);
    void addOption(const QString& flag, ExpressionNode *expr);

private:
    QString _command;
    QHash< QString, QSharedPointer<ExpressionNode> > _options;
};
}

#endif // COMMANDNODE_H
