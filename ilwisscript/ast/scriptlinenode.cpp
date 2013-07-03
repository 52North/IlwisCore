#include <map>
#include "ilwis.h"
#include "symboltable.h"
#include "astnode.h"
#include "idnode.h"
#include "formatter.h"
#include "scriptnode.h"
#include "scriptlinenode.h"

using namespace Ilwis;

ScriptLineNode::ScriptLineNode()
{
}

QString ScriptLineNode::nodeType() const
{
    return "scriptline";
}

bool ScriptLineNode::evaluate(SymbolTable &symbols, int scope )
{
    foreach(QSharedPointer<ASTNode> node, _childeren) {
        if ( node->nodeType() == "formatnode") {
            Formatter *fnode = static_cast<Formatter *>(node.data());
            if ( fnode->dataType() == "gridcoverage")
                ScriptNode::setActiveFormat(itGRID, node)   ;

        }
        if (!node->evaluate(symbols, scope)) {
            return false;
        }
    }
    _evaluated = true;
    return _evaluated;
}
