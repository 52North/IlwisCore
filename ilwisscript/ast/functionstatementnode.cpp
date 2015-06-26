#include "ilwis.h"
#include "symboltable.h"
#include "astnode.h"
#include "idnode.h"
#include "parametersnode.h"
#include "commandhandler.h"
#include "functionstatementnode.h"

using namespace Ilwis;

FunctionStatementNode::FunctionStatementNode(char *s) : IDNode(s)
{
}

void FunctionStatementNode::setParameters(ParametersNode *parm)
{
    _parameters = QSharedPointer<ParametersNode>(parm);
}

QString FunctionStatementNode::nodeType() const
{
    return "functionStatement";
}

bool FunctionStatementNode::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx)
{
    QString parms;
    if ( !_parameters.isNull()){
        for(int i=0; i < _parameters->noOfChilderen(); ++i) {
            bool ok = _parameters->child(i)->evaluate(symbols, scope, ctx);

            if (!ok)
                return false;
            auto node = _parameters->child(i)->value();
            QString name = node.toString();
            if ( name == sUNDEF)
                name = node.id();
            //QString name = getName(_parameters->child(i)->value());
            if ( parms.size() != 0)
                parms += ",";
            parms += name;

        }


    }
    QString exp = QString("%1(%2)").arg(id()).arg(parms);
    return commandhandler()->execute(exp,ctx, symbols);
}
