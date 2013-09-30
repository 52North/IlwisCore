#ifndef RELATIONNODE_H
#define RELATIONNODE_H

namespace Ilwis {
class RelationNode : public OperationNode
{
public:
    RelationNode();
     QString nodeType() const;
     bool evaluate(SymbolTable& symbols, int scope, ExecutionContext *ctx);
private:
     bool handleEQ(const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
     bool handleNEQ(const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
     bool handleGREATEREQ(const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
     bool handleGREATER(const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
     bool handleLESS(const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
     bool handleLESSEQ(const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
};
}

#endif // RELATIONNODE_H
