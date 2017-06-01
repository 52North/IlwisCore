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
    OperationResource operation({"ilwis://operations/text2output"});
    operation.setLongName("text2output");
    operation.setSyntax("text2output(text[,text])");
    operation.setInParameterCount({1,2});
    operation.addInParameter(0,itANY, TR("input string"),TR("input string"));
    operation.addOptionalInParameter(1,itANY, TR("filename or path"),TR("optional file where strings will be written; if no path is provided, current working folder will be used"));
    operation.setOutParameterCount({0});
    operation.setKeywords("text, output,internal");

    mastercatalog()->addItems({operation});
    return operation.id();
}
