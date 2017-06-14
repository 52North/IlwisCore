#include <iostream>
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "stringoperations.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(StringFind)

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
    ctx->setOutput(symTable, value, sUNDEF, itINT32, Resource());

    return true;
}

Ilwis::OperationImplementation *StringFind::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new StringFind(metaid, expr);
}

Ilwis::OperationImplementation::State StringFind::prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    if (_expression.parameterCount() < 2) {
        return sPREPAREFAILED;
    }
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
    return sPREPARED;
}

quint64 StringFind::createMetadata()
{
    OperationResource resource({"ilwis://operations/stringfind"});
    resource.setLongName("stringfind");
    resource.setSyntax("stringfind(source,searchtext,[,begin])");
    resource.setInParameterCount({2,3});
    resource.addInParameter(0, itANY,TR("input string"), TR("input string"));
    resource.addInParameter(1, itANY, TR("search string"), TR("string to be found in in the input string"));
    resource.addOptionalInParameter(2, itINT32, TR("start_index"), TR("optional index from where to begin the search"));
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itINT32, TR("output index"), TR("index where the to be found string is found(0 based) or -1 if it can not be found"));
    resource.setKeywords("string,workflow");

    //resource.prepare();
    //url += "=" + QString::number(resource.id());
    //resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();
}

//--------------------------------------------------------------------------------------------------------
REGISTER_OPERATION(StringSub)

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
    QString name = _expression.parameterCount(false) > 0
            ? _expression.parm(0, false).value()
            : sUNDEF;

    ctx->setOutput(symTable, value, name, itSTRING, Resource());

    return true;
}

Ilwis::OperationImplementation *StringSub::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new StringSub(metaid, expr);
}

Ilwis::OperationImplementation::State StringSub::prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
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
        _end = _end < 0 ?  _text.size() : std::min(_end, _text.size());
    }
    return sPREPARED;
}

quint64 StringSub::createMetadata()
{
    OperationResource resource({"ilwis://operations/stringsub"});
    resource.setLongName("stringsub");
    resource.setSyntax("stringsub(source,begin,[,end])");
    resource.setInParameterCount({2,3});
    resource.addInParameter(0, itANY, TR("input string"),TR("input string"));
    resource.addInParameter(1, itINT32, TR("begin"),TR("start index of the substring in the input string"));
    resource.addOptionalInParameter(2, itINT32, TR("end"),TR("optional count of characters to end the substring, if not present the string will go until the end"));
    resource.setOutParameterCount({1});
    resource.addOutParameter(0,itSTRING,TR("sub string"),TR("a sub string from the input string"));

    resource.setKeywords("string,workflow");

    mastercatalog()->addItems({resource});
    return resource.id();
}

//---------------------------------------------------------------------------------
REGISTER_OPERATION(StringReplace)

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
    ctx->setOutput(symTable, value, sUNDEF, itSTRING, Resource());

    return true;
}

Ilwis::OperationImplementation *StringReplace::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new StringReplace(metaid, expr);
}

Ilwis::OperationImplementation::State StringReplace::prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    if (_expression.parameterCount() < 3) {
        return sPREPAREFAILED;
    }
    _text = _expression.parm(0).value();
    _searchText = _expression.parm(1).value();
    _replaceText = _expression.parm(2).value();
    return sPREPARED;
}

quint64 StringReplace::createMetadata()
{
    OperationResource resource({"ilwis://operations/stringreplace"});
    resource.setLongName("stringreplace");
    resource.setSyntax("StringReplace(source,searchtext,replacetext)");
    resource.setInParameterCount({3});
    resource.addInParameter(0, itANY, TR("input string"), TR("input string"));
    resource.addOptionalInParameter(1, itSTRING, TR("search string"), TR("string to be found in in the input string"));
    resource.addOptionalInParameter(2, itSTRING, TR("replace string"),TR("the search string will be replaced with the replace string"));
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itINT32, TR("output index"), TR("index where the to be found string is found(0 based) or -1 if it can not be found"));

    resource.setKeywords("string,workflow");

    mastercatalog()->addItems({resource});
    return resource.id();
}

//---------------------------------------------------------------------------------
REGISTER_OPERATION(StringAdd)

StringAdd::StringAdd()
{
}

StringAdd::StringAdd(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool StringAdd::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    QString newString = _text1 + _text2;
    QVariant value = newString;
    logOperation(_expression);
    ctx->setOutput(symTable, value, sUNDEF, itSTRING, Resource());

    return true;
}

Ilwis::OperationImplementation *StringAdd::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new StringAdd(metaid, expr);
}

Ilwis::OperationImplementation::State StringAdd::prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    _text1 = _expression.parm(0).value();
    _text2 = _expression.parm(1).value();
    return sPREPARED;
}

quint64 StringAdd::createMetadata()
{
    OperationResource resource({"ilwis://operations/addstrings"});
    resource.setLongName("addstrings");
    resource.setSyntax("addstrings(text1, text2)");
    resource.setInParameterCount({2});
    resource.addInParameter(0, itSTRING, TR("first string"), TR("beginning of the output string"));
    resource.addInParameter(1, itSTRING, TR("second string"), TR("end of the output string"));
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itSTRING, TR("output string"), TR("concattanation of the two input string"));

    resource.setKeywords("string,workflow");

    mastercatalog()->addItems({resource});
    return resource.id();
}
