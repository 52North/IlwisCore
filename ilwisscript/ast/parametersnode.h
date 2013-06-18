#ifndef PARAMETERSNODE_H
#define PARAMETERSNODE_H

namespace Ilwis {
class ParametersNode : public ASTNode
{
public:
    ParametersNode();
    QString nodeType() const;
};
}

#endif // PARAMETERSNODE_H
