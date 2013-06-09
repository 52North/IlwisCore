#ifndef DOMPARTNODE_H
#define DOMPARTNODE_H

class DomPartNode : public ASTNode
{
public:
    DomPartNode();
    void setDomainId(IDNode *node);
    void setValueRange(ValueRangeNode *node);
    QString nodeType() const;

private:
    QSharedPointer<IDNode> _domainId;
    QSharedPointer<ValueRangeNode> _valrange;
};

#endif // DOMPARTNODE_H
