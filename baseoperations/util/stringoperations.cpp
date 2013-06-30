#include <iostream>
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "OperationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "commandhandler.h"
#include "stringoperations.h"

using namespace Ilwis;
using namespace BaseOperations;

StringFind::StringFind()
{
}

StringFind::StringFind(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool StringFind::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    int n = _text.indexOf(_searchText,_start);
    QVariant value = n;
    ctx->addOutput(symTable, value, sUNDEF, itINT32, Resource());

    return true;
}

Ilwis::OperationImplementation *StringFind::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new StringFind(metaid, expr);
}

Ilwis::OperationImplementation::State StringFind::prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &)
{
    _text = _expression.parm(0).value();
    _searchText = _expression.parm(1).value();
    if ( _expression.parameterCount() == 3) {
        bool ok;
        _start = _expression.parm(2).value().toLong(&ok);
        if (!ok) {
            ERROR2(ERR_ILLEGAL_VALUE_2,"index","stringfind");
            return sPREPAREFAILED;
        }
        _start = std::max(0, _start);
    }
    return sPREPAREFAILED;
}

quint64 StringFind::createMetadata()
{
    QString url = QString("ilwis://operations/stringfind");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","stringfind");
    res.addProperty("syntax","stringfind(source,searchtext,[,begin])");
    res.addProperty("inparameters","2|3");
    res.addProperty("pin_1_type", itANY);
    res.addProperty("pin_1_name", TR("input string"));
    res.addProperty("pin_1_desc",TR("input string"));
    res.addProperty("pin_2_type", itANY);
    res.addProperty("pin_2_name", TR("search string"));
    res.addProperty("pin_2_desc",TR("string to be found in in the input string"));
    res.addProperty("pin_3_type", itINT32);
    res.addProperty("pin_3_name", TR("start_index"));
    res.addProperty("pin_3_desc",TR("optional index from where to begin the search"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_name", TR("output index"));
    res.addProperty("pout_1_type", itINT32);
    res.addProperty("pout_1_desc",TR("index where the to be found string is found(0 based) or -1 if it can not be found"));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();
}

//--------------------------------------------------------------------------------------------------------
StringSub::StringSub()
{
}

StringSub::StringSub(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool StringSub::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    QString sub = _text.mid(_start,_end);

    QVariant value = sub;
    ctx->addOutput(symTable, value, sUNDEF, itSTRING, Resource());

    return true;
}

Ilwis::OperationImplementation *StringSub::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new StringSub(metaid, expr);
}

Ilwis::OperationImplementation::State StringSub::prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &)
{
    _text = _expression.parm(0).value();
    bool ok;
    _start = _expression.parm(1).value().toLong(&ok);
    if (!ok) {
        ERROR2(ERR_ILLEGAL_VALUE_2,"start index","StringSub");
        return sPREPAREFAILED;
    }
    if ( _expression.parameterCount() == 3) {

        _end = _expression.parm(2).value().toLong(&ok);
        if (!ok) {
            ERROR2(ERR_ILLEGAL_VALUE_2,"end index","StringSub");
            return sPREPAREFAILED;
        }
        _start = std::max(0, _start);
        _end = _end < 0 ?  _text.size() : std::max (_end, _text.size());
    }
    return sPREPARED;
}

quint64 StringSub::createMetadata()
{
    QString url = QString("ilwis://operations/stringsub");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","stringsub");
    res.addProperty("syntax","stringsub(source,begin,[,end])");
    res.addProperty("inparameters","2|3");
    res.addProperty("pin_1_type", itANY);
    res.addProperty("pin_1_name", TR("input string"));
    res.addProperty("pin_1_desc",TR("input string"));
    res.addProperty("pin_2_type", itINT32);
    res.addProperty("pin_2_name", TR("begin"));
    res.addProperty("pin_2_desc",TR("start index of the substring in the input string"));
    res.addProperty("pin_3_type", itINT32);
    res.addProperty("pin_3_name", TR("end"));
    res.addProperty("pin_3_desc",TR("optional index from where to end the substring, if not present the string will go until the end"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_name", TR("sub string"));
    res.addProperty("pout_1_type", itSTRING);
    res.addProperty("pout_1_desc",TR("a sub string from the input string"));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();
}

//---------------------------------------------------------------------------------
StringReplace::StringReplace()
{
}

StringReplace::StringReplace(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool StringReplace::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    QString newString = _text.replace(_searchText, _replaceText);
    QVariant value = newString;
    ctx->addOutput(symTable, value, sUNDEF, itSTRING, Resource());

    return true;
}

Ilwis::OperationImplementation *StringReplace::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new StringReplace(metaid, expr);
}

Ilwis::OperationImplementation::State StringReplace::prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &)
{
    _text = _expression.parm(0).value();
    _searchText = _expression.parm(1).value();
    _replaceText = _expression.parm(2).value();
    return sPREPAREFAILED;
}

quint64 StringReplace::createMetadata()
{
    QString url = QString("ilwis://operations/stringreplace");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","stringreplace");
    res.addProperty("syntax","StringReplace(source,searchtext,replacetext)");
    res.addProperty("inparameters","3");
    res.addProperty("pin_1_type", itANY);
    res.addProperty("pin_1_name", TR("input string"));
    res.addProperty("pin_1_desc",TR("input string"));
    res.addProperty("pin_2_type", itSTRING);
    res.addProperty("pin_2_name", TR("search string"));
    res.addProperty("pin_2_desc",TR("string to be found in in the input string"));
    res.addProperty("pin_3_type", itSTRING);
    res.addProperty("pin_3_name", TR("replace string"));
    res.addProperty("pin_3_desc",TR("the search string will be replaced with the replace string"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_name", TR("output index"));
    res.addProperty("pout_1_type", itINT32);
    res.addProperty("pout_1_desc",TR("index where the to be found string is found(0 based) or -1 if it can not be found"));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();
}
