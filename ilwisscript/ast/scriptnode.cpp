#include <map>
#include "ilwis.h"
#include "symboltable.h"
#include "astnode.h"
#include "idnode.h"
#include "formatter.h"
#include "scriptnode.h"

using namespace Ilwis;

std::map<quint64, QSharedPointer<ASTNode> > ScriptNode::_activeFormat;

ScriptNode::ScriptNode()
{
}

QString ScriptNode::nodeType() const
{
    return "script";
}

Formatter * ScriptNode::activeFormat(IlwisTypes type)
{
    auto iter= _activeFormat.find(type);
    if ( iter != _activeFormat.end())
        return static_cast<Formatter *>((*iter).second.data());

    return 0;
}

void ScriptNode::setActiveFormat(quint64 tp, const QSharedPointer<ASTNode> &node)
{
    _activeFormat[tp] = node;
}
