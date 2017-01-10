#ifndef ADDNODE_H
#define ADDNODE_H

namespace Ilwis {
class AddNode : public OperationNodeScript
{
public:
    AddNode();
    QString nodeType() const;
    bool evaluate(SymbolTable& symbols, int scope, ExecutionContext *ctx);
private:
    bool handleAdd(int index,const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
    bool handleSubstract(int index, const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
};
}

#endif // ADDNODE_H
