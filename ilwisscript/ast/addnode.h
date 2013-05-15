#ifndef ADDNODE_H
#define ADDNODE_H

class AddNode : public OperationNode
{
public:
    AddNode();
    QString nodeType() const;
    bool evaluate(SymbolTable& symbols, int scope);
private:
    bool handleAdd(const NodeValue &vright);
    bool handleSubstract(const NodeValue &vright);
};

#endif // ADDNODE_H
