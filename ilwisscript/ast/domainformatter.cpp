#include "ilwis.h"
#include "symboltable.h"
#include "astnode.h"
#include "idnode.h"
#include "valuerangenode.h"
#include "dompartnode.h"

DomPartNode::DomPartNode()
{
}

void DomPartNode::setDomainId(IDNode *node)
{
    _domainId = QSharedPointer<IDNode>(node);
}

void DomPartNode::setValueRange(ValueRangeNode *node)
{
    _valrange = QSharedPointer<ValueRangeNode>(node);

}

QString DomPartNode::nodeType() const
{
    return "dompart";
}
