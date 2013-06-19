#ifndef ADDNODE_H
#define ADDNODE_H

namespace Ilwis {
class AddNode : public OperationNode
{
public:
    AddNode();
    QString nodeType() const;
    bool evaluate(SymbolTable& symbols, int scope);
private:
    bool handleAdd(const NodeValue &vright, Ilwis::SymbolTable &symbols);
    bool handleSubstract(const NodeValue &vright, Ilwis::SymbolTable &symbols);
};
}

#endif // ADDNODE_H
