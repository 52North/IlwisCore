#ifndef MULTIPLICATIONNODE_H
#define MULTIPLICATIONNODE_H

namespace Ilwis {
class MultiplicationNode : public OperationNode
{
public:
    MultiplicationNode();
    QString nodeType() const;
    bool evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx);
private:
    bool handleMod(const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
    bool handleDiv(const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
    bool handleTimes(const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
};
}

#endif // MULTIPLICATIONNODE_H
