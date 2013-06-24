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

bool FunctionStatementNode::evaluate(SymbolTable &symbols, int scope)
{
    QString parm;
    if ( !_parameters.isNull()){
        _parameters->evaluate(symbols, scope);
        auto val = _parameters->value();
        auto values = val.value<QVariantList>();
        for(const auto& var : values) {
            NodeValue nvalue = var.value<NodeValue>();
            if ( parm != "")
                parm += ",";
            if ( nvalue.content() == NodeValue::ctString){
                parm += nvalue.value<QString>();
            }
            if ( nvalue.content() == NodeValue::ctID) {
                QString val = nvalue.toString();
                parm += val;
            }
        }


    }
    QString exp = QString("%1(%2)").arg(id()).arg(parm);
    ExecutionContext ctx;
    return commandhandler()->execute(exp,&ctx, symbols);
}
