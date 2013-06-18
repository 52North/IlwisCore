#include "ilwis.h"
#include "symboltable.h"
#include "astnode.h"
#include "idnode.h"
#include "valuerangenode.h"
#include "domainformatter.h"

using namespace Ilwis;

DomainFormatter::DomainFormatter()
{
}

void DomainFormatter::setDomainId(IDNode *node)
{
    _domainId = QSharedPointer<IDNode>(node);
}

void DomainFormatter::setValueRange(ValueRangeNode *node)
{
    _valrange = QSharedPointer<ValueRangeNode>(node);

}

QString DomainFormatter::nodeType() const
{
    return "dompart";
}
