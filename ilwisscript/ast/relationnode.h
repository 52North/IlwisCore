#ifndef RELATIONNODE_H
#define RELATIONNODE_H

class RelationNode : public OperationNode
{
public:
    RelationNode();
     QString nodeType() const;
     bool evaluate(SymbolTable& symbols, int scope);
private:
     bool handleEQ(const NodeValue &vright);
     bool handleNEQ(const NodeValue &vright);
     bool handleGREATEREQ(const NodeValue &vright);
     bool handleGREATER(const NodeValue &vright);
     bool handleLESS(const NodeValue &vright);
     bool handleLESSEQ(const NodeValue &vright);
};

#endif // RELATIONNODE_H
