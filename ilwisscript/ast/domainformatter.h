#ifndef DomainFormatter_H
#define DomainFormatter_H

namespace Ilwis {
class DomainFormatter : public ASTNode
{
public:
    DomainFormatter();
    void setDomainId(IDNode *node);
    void setValueRange(ValueRangeNode *node);
    QString nodeType() const;

private:
    QSharedPointer<IDNode> _domainId;
    QSharedPointer<ValueRangeNode> _valrange;
};
}

#endif // DomainFormatter_H
