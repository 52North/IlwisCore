#ifndef FUNCTIONSTATEMENTNODE_H
#define FUNCTIONSTATEMENTNODE_H

namespace Ilwis {
class FunctionStatementNode : public IDNode
{
public:
    FunctionStatementNode(char *s);
    void setParameters(ParametersNode *parm);
    QString nodeType() const;

private:
    QSharedPointer<ParametersNode> _parameters;

};
}

#endif // FUNCTIONSTATEMENTNODE_H
