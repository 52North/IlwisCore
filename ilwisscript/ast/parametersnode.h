#ifndef PARAMETERSNODE_H
#define PARAMETERSNODE_H

namespace Ilwis {
class ParametersNode : public ASTNode
{
public:
    ParametersNode();
    QString nodeType() const;
    bool evaluate(SymbolTable &symbols, int scope);
};
}

#endif // PARAMETERSNODE_H
