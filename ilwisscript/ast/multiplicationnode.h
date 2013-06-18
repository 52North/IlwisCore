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
    bool handleMod(const NodeValue &vright);
    bool handleDiv(const NodeValue &vright);
    bool handleTimes(const NodeValue &vright);
};
}

#endif // MULTIPLICATIONNODE_H
