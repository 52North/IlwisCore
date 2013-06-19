#ifndef RELATIONNODE_H
#define RELATIONNODE_H

namespace Ilwis {
class RelationNode : public OperationNode
{
public:
    RelationNode();
     QString nodeType() const;
     bool evaluate(SymbolTable& symbols, int scope);
private:
     bool handleEQ(const NodeValue &vright, Ilwis::SymbolTable &symbols);
     bool handleNEQ(const NodeValue &vright, Ilwis::SymbolTable &symbols);
     bool handleGREATEREQ(const NodeValue &vright, Ilwis::SymbolTable &symbols);
     bool handleGREATER(const NodeValue &vright, Ilwis::SymbolTable &symbols);
     bool handleLESS(const NodeValue &vright, Ilwis::SymbolTable &symbols);
     bool handleLESSEQ(const NodeValue &vright, Ilwis::SymbolTable &symbols);
};
}

#endif // RELATIONNODE_H
