#ifndef FUNCTIONNODE_H
#define FUNCTIONNODE_H

namespace Ilwis{
class FunctionNode : public IDNode
{
public:
    FunctionNode(char *s);

    void setId(IDNode *node);
    void addParameter(VariableNode * node);
    void setReturn(ReturnNode *node);
    QString nodeType() const;

private:
    QSharedPointer<IDNode> _id;
    QVector< QSharedPointer<VariableNode> > _parameters;
    QSharedPointer<ReturnNode> _return;
};
}
#endif // FUNCTIONNODE_H
