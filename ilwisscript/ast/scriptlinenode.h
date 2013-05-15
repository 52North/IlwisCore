#ifndef SCRIPTLINENODE_H
#define SCRIPTLINENODE_H

class ScriptLineNode : public ASTNode
{
public:
    ScriptLineNode();
    QString nodeType() const;
    bool evaluate(SymbolTable &symbols, int scope);
};

#endif // SCRIPTLINENODE_H
