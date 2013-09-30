#ifndef ADDNODE_H
#define ADDNODE_H

namespace Ilwis {
class AddNode : public OperationNode
{
public:
    AddNode();
    QString nodeType() const;
    bool evaluate(SymbolTable& symbols, int scope, ExecutionContext *ctx);
private:
    bool handleAdd(const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
    bool handleSubstract(const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
    IlwisTypes typesUsed(const NodeValue &vright, Ilwis::SymbolTable &symbols) const;
};
}

#endif // ADDNODE_H
