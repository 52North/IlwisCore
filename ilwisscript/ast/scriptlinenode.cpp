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

bool ScriptLineNode::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx )
{
     _evaluated = true;
    foreach(QSharedPointer<ASTNode> node, _childeren) {
        if ( node->nodeType() == "formatnode") {
            Formatter *fnode = static_cast<Formatter *>(node.data());
            if ( fnode->dataType() == "gridcoverage")
                ScriptNode::setActiveFormat(itRASTER, node)   ;

        }
        if (!node->evaluate(symbols, scope, ctx)) {
            _evaluated =  false;
            break;
        }
    }
    // we do not keep al binary data in memory for rasters as within a script they might not get out of scope until
    // the script finishes. This quickly fills up memory. So we unload all binaries and the raster will reload when it
    // is needed (if it all). This means a slight performance hit but it is necessary
    symbols.unloadData();

    return _evaluated;
}
