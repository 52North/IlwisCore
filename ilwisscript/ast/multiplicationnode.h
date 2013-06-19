#ifndef MULTIPLICATIONNODE_H
#define MULTIPLICATIONNODE_H

namespace Ilwis {
class MultiplicationNode : public OperationNode
{
public:
    MultiplicationNode();
    QString nodeType() const;
    bool evaluate(SymbolTable &symbols, int scope);
private:
    bool handleMod(const NodeValue &vright, Ilwis::SymbolTable &symbols);
    bool handleDiv(const NodeValue &vright, Ilwis::SymbolTable &symbols);
    bool handleTimes(const NodeValue &vright, Ilwis::SymbolTable &symbols);
};
}

#endif // MULTIPLICATIONNODE_H
