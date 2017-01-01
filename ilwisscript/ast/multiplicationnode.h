#ifndef MULTIPLICATIONNODE_H
#define MULTIPLICATIONNODE_H

namespace Ilwis {
class MultiplicationNode : public OperationNodeScript
{
public:
    MultiplicationNode();
    QString nodeType() const;
    bool evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx);
private:
    bool handleMod(int index, const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
    bool handleDiv(int index, const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
    bool handleTimes(int index, const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
};
}

#endif // MULTIPLICATIONNODE_H
