#include <map>
#include "ilwis.h"
#include "symboltable.h"
#include "astnode.h"
#include "idnode.h"
#include "formatter.h"
#include "scriptnode.h"
#include "scriptlinenode.h"

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
                ScriptNode::setActiveFormat(itGRIDCOVERAGE, node)   ;

        }
        if (!node->evaluate(symbols, scope)) {
            return false;
        }
    }
    _evaluated = true;
    return _evaluated;
}
