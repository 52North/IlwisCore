#include "kernel.h"
#include "errorobject.h"
#include "geometries.h"
#include "astnode.h"
#include "idnode.h"
#include "selectornode.h"
#include "outparametersnode.h"

using namespace Ilwis;

OutParametersNode::OutParametersNode()
{
}

void OutParametersNode::addSelector(const QString& id, Selector *n)
{
    _selectors[id]  = QSharedPointer<Selector>(n);
}

void OutParametersNode::addResult(IDNode *n)
{
    _ids.push_back(QSharedPointer<IDNode>(n));
}

void OutParametersNode::addSpecifier(const QString& id, ASTNode *n)
{
    _specifiers[id]  = QSharedPointer<ASTNode>(n);
}

QSharedPointer<Selector> OutParametersNode::selector(const QString& id) const
{
    auto iter = _selectors.find(id);
    if ( iter != _selectors.end())
        return (*iter).second;
    return QSharedPointer<Selector>();
}

QString OutParametersNode::id(int index) const
{
    if ( index < _ids.size())
        return _ids[index]->id();
    return sUNDEF;
}

QSharedPointer<ASTNode> OutParametersNode::specifier(const QString& id) const
{
    auto iter = _specifiers.find(id);
    if ( iter != _specifiers.end())
        return (*iter).second;
    return QSharedPointer<ASTNode>();
}


