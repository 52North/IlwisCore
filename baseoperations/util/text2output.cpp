#include <iostream>
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "text2output.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(Text2Output)

Text2Output::Text2Output()
{
}

Text2Output::Text2Output(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

bool Text2Output::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    (*ctx->_out) << _text.toStdString() << "\n"    ;

    return true;
}

Ilwis::OperationImplementation *Text2Output::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Text2Output(metaid, expr);
}

Ilwis::OperationImplementation::State Text2Output::prepare(ExecutionContext *, const Ilwis::SymbolTable &symTable)
{
    for(int i=0; i < _expression.parameterCount(); ++i){
        const Parameter& parm = _expression.parm(i);
        QString str = sUNDEF;
        if ( parm.valuetype() == itSTRING){
          str = parm.value();
          QString var = symTable.getValue(str).toString();
          if ( var != "")
              str = var;
        }
        else {
            Symbol sym = symTable.getSymbol(parm.value());
            if ( sym._type & itNUMBER){
                QString tp = sym._var.typeName();
                if ( tp == "QVariantList"){
                    QVariantList lst = sym._var.value<QVariantList>();
                    str =  lst[0].toString();
                }else
                    str = sym._var.toString();
            }
        }

        if ( str.size() > 0 && str[0] == '"' && str[str.size()-1] == '"'){
            str = str.remove('"');
        }
        _text += str;
    }
    return sPREPARED;

}

quint64 Text2Output::createMetadata()
{
    QString url = QString("ilwis://operations/text2output");
    Resource resource(QUrl(url), itOPERATIONMETADATA);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","text2output");
    resource.addProperty("syntax","text2output(text,[text]+)");
    resource.addProperty("inparameters","1+");
    resource.addProperty("pin_1_type", itANY);
    resource.addProperty("pin_1_name", TR("input string"));
    resource.addProperty("pin_1_desc",TR("input string"));
    resource.addProperty("pin_2_type", itANY);
    resource.addProperty("pin_2_name", TR("filename or path"));
    resource.addProperty("pin_2_desc",TR("optional file were strings will be written; if no path is provided, current working folder will be used"));
    resource.addProperty("outparameters",0);
    resource.prepare();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();
}
