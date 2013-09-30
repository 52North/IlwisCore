#ifndef SCRIPTLINENODE_H
#define SCRIPTLINENODE_H

namespace Ilwis {
class ScriptLineNode : public ASTNode
{
public:
    ScriptLineNode();
    QString nodeType() const;
    bool evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx);
};
}

#endif // SCRIPTLINENODE_H
