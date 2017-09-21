#include <functional>
#include <future>
#include <QThread>
#include <QCoreApplication>
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "domainitem.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "itemdomain.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "numericoperation.h"
#include "calculatoroperation.h"

using namespace Ilwis;
using namespace BaseOperations;

CalculatorOperation::CalculatorOperation()
{}

CalculatorOperation::CalculatorOperation(quint64 metaid,const Ilwis::OperationExpression &expr) : NumericOperation(metaid, expr)
{
    _functions ={{"iff",3},{"sin",1},{"cos",1},{"tan",1},{"asin",1},{"acos",1},{"atan",1},
                 {"log10",1},{"ln",1},{"abs",1},{"ciel",1},{"floor",1},{"sqrt",1},{"max",2},
                 {"min",2},{"pow",2}};
    _operators["+"] = { 2, LEFT_ASSOC };
    _operators["-"] = { 2, LEFT_ASSOC };
    _operators["*"] = { 5, LEFT_ASSOC };
    _operators["/"] = { 5, LEFT_ASSOC };
    _operators[">"] = { 1, LEFT_ASSOC };
    _operators["<"] = { 1, LEFT_ASSOC };
    _operators["<="] = { 1, LEFT_ASSOC };
    _operators[">="] = { 1, LEFT_ASSOC };
    _operators["!="] = { 1, LEFT_ASSOC };
    _operators["=="] = { 1, LEFT_ASSOC };
    _operators["and"] = { 0, LEFT_ASSOC };
    _operators["or"] = { 0, LEFT_ASSOC };
}

OperationImplementation::State  CalculatorOperation::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    return sPREPAREFAILED;
}

QStringList CalculatorOperation::tokenizer(const QString &expr)
{
    QStringList tokens;
    std::vector<QString> seperators1 = {"*", "+","-","/","(",")","<",">",","};
    std::vector<QString> seperators2 = {"==","!=",">=","<=","and","or"};
    QString token;
    bool inQuotes = false;
    for(int i=0; i < expr.size(); ++i){
        QChar c = expr[i];
        QChar lookAhead =  i < expr.size() - 1 ? expr[i+1] : ' ';
        if ( c == ' ')
            continue;
        if ( c == '\''){
            if ( inQuotes){
                tokens.push_back("'" + token + "'");
                token = "";
            }
            inQuotes = !inQuotes;
            continue;
        }
        QString sep2 = QString(c) + QString(lookAhead).trimmed();
        auto iter2 = std::find(seperators2.begin(), seperators2.end(), sep2);
        if (iter2 != seperators2.end()){
            if (token != ""){
                tokens.push_back(token);
                token = "";
            }
            tokens.push_back(sep2);
            i+=1;
        }else{
            QString sep1 = QString(c);
            auto iter1 = std::find(seperators1.begin(), seperators1.end(), sep1);
            if (iter1 != seperators1.end()){
                if (token != "")
                    tokens.push_back(token);
                tokens.push_back(sep1);
                token = "";
            }else{
                token += c;
            }
        }
    }
    if (token != "")
        tokens.push_back(token);
    return tokens;
}

QStringList CalculatorOperation::shuntingYard(const QString &expr)
{
    QStringList rpn;
    std::stack<QString> tokenstack;

    QStringList tokens = tokenizer(expr);

    for(QString token : tokens){
        if ( token == "")
            continue;
        if ( isOperator(token)){
            while(!tokenstack.empty() && isOperator(tokenstack.top())){
                if(isAssociative(token, LEFT_ASSOC) &&
                        (cmpPrecedence(token, tokenstack.top()) <=0) ||
                        (isAssociative(token, RIGHT_ASSOC)&&
                         cmpPrecedence(token, tokenstack.top()) < 0))
                {
                    rpn.push_back(tokenstack.top());
                    tokenstack.pop();
                    continue;
                }
                break;
            }
            tokenstack.push(token);
        }else if ( token == "," ){
            while(!tokenstack.empty() && tokenstack.top() != "("){
                rpn.push_back(tokenstack.top());
                tokenstack.pop();
            }
        }else if ( token == "("){
            tokenstack.push(token);
        }else if ( token == ")"){
            while (!tokenstack.empty() && tokenstack.top() != "(")
            {
                rpn.push_back(tokenstack.top());
                tokenstack.pop();
            }
            check(!tokenstack.empty(),TR("Illegal construct in expression; maybe missing brackets?"));
            tokenstack.pop();
            if (!tokenstack.empty() && isFunction(tokenstack.top())){
                rpn.push_back(tokenstack.top());
                tokenstack.pop();
            }
        }else if (isFunction(token)){
            tokenstack.push(token);
        }else{
            rpn.push_back(token);
        }

    }
    while (!tokenstack.empty())
    {
        rpn.push_back(tokenstack.top());
        tokenstack.pop();
    }

    return rpn;
}

CalculatorOperation::MathAction CalculatorOperation::string2action(const QString& action){
    if ( action == "+") return maADD;
    if ( action == "-") return maMINUS;
    if ( action == "/") return maDIVIDE;
    if ( action == "*") return maMULT;
    if ( action == "iff") return maIFF;
    if ( action == "sin") return maSIN;
    if ( action == "cos") return maCOS;
    if ( action == "tan") return maTAN;
    if ( action == "asin") return maASIN;
    if ( action == "acos") return maACOS;
    if ( action == "atan") return maATAN;
    if ( action == "pow") return maPOW;
    if ( action == "ln") return maLN;
    if ( action == "log10") return maLOG10;
    if ( action == "max") return maMAX;
    if ( action == "min") return maMIN;
    if ( action == "ceil") return maCEIL;
    if ( action == "floor") return maFLOOR;
    if ( action == "abs") return maABS;
    if ( action == "sqrt") return maSQRT;
    if ( action == "==") return maEQ;
    if ( action == "!=") return maNEQ;
    if ( action == "<=") return maLESSEQ;
    if ( action == ">=") return maGREATEREQ;
    if ( action == "<") return maLESS;
    if ( action == ">") return maGREATER;
    if ( action == "and") return maAND;
    if ( action == "or") return maOR;

    return maUNKNOWN;
}

bool CalculatorOperation::isFunction(const QString& func){
    auto iter = _functions.find(func.toLower());
    return iter != _functions.end();
}

bool CalculatorOperation::isNumber(const QString &token) const
{
    bool ok = false;
    token.toUInt(&ok);
    return ok;
}

bool CalculatorOperation::isOperator(const QString& token)
{
    auto iter = _operators.find(token);
    bool ok = iter != _operators.end();
    return ok;
}

bool CalculatorOperation::isAssociative(const QString& token, int type)
{
    check(isOperator(token),TR("Illegal construct in expression; Invalid token: " + token));
    if (_operators[token][1] == type) {
        return true;
    }
    return false;
}

int CalculatorOperation::cmpPrecedence(const QString& token1, const QString& token2)
{
    check(isOperator(token1),TR("Illegal construct in expression; Invalid token: " + token1));
    check(isOperator(token2),TR("Illegal construct in expression; Invalid token: " + token2));

    return _operators[token1][0] - _operators[token2][0];
}

int  CalculatorOperation::checkItem(int domainCount, QString& item, QString& copy_item, std::set<QString>& domainItems){
    if (copy_item.indexOf("LINK:") == -1){
        check(copy_item.size() > 0, TR("invalid syntax"));
        if ( copy_item[0] == '\''){
            // add a prefix to the string to be able to link it to a domain.
            domainItems.insert(item.mid(1,item.size() - 2));
            item = "DOMAIN:"+ QString::number(domainCount)+":" + item;
        }
        return -1;
    }else {
        check(copy_item.size()>5, TR("invalid syntax"));
        int nextLink = copy_item.mid(5).toInt();
        // remove the link as we dont want to encounter when we do a subsequent run through the list of tokens
        copy_item = sUNDEF;
        return nextLink;
    }
}



IDomain CalculatorOperation::collectDomainInfo(std::vector<std::vector<QString>>& rpn){
    int domainCount = 0;
    std::vector<std::vector<QString>> copy_rpn = rpn;
    int maxLink = -1;
    int index = -1;
    std::map<int,IDomain> itemdomains;
    bool found = false;
    do{
        found = false;
        for(int i=0; i < copy_rpn.size(); ++i )    {
            auto& copy_item = copy_rpn[i];
            auto& item = rpn[i];
            check(copy_item.size()>0, TR("invalid syntax"));
            if ( copy_item[0] == "iff"){
                QString cItem = copy_item[1];
                if (cItem.indexOf("LINK:") == 0){
                    maxLink = std::max(cItem.mid(5).toInt(), maxLink);
                    index = i;
                    found = true;
                }
            }else {
                check(item.size()>0, TR("invalid syntax"));
                if ( item[0] == "=="){ // @1='sometext' or 'sometext=@1'
                    check(item.size()>1 && item[1].size() > 0, TR("invalid syntax"));
                    if ( item[1][0] == '\''){
                        check(item.size()>2 && item[2].size() > 0, TR("invalid syntax"));
                        if ( item[2][0] == '@'){
                            int index = item[2].mid(1).toInt();
                            DataDefinition def = datadef(index);
                            if ( def.isValid())
                                if ( def.domain()->ilwisType() == itITEMDOMAIN){
                                    _domains[domainCount] =def.domain();
                                    rpn[i][1] = "DOMAIN:"+ QString::number(domainCount++) +":" + rpn[i][1];
                                }
                        }
                    }else  {
                        check(item.size()>2 && item[2].size() > 0, TR("invalid syntax"));
                        if ( item[2][0] == '\''){
                            check(item[1].size() > 0, TR("invalid syntax"));
                            if ( item[1][0] == '@'){
                                int index = item[2].mid(1).toInt();
                                DataDefinition def = datadef(index);
                                if ( def.isValid())
                                    if ( def.domain()->ilwisType() == itITEMDOMAIN){
                                        _domains[domainCount] = def.domain();
                                        rpn[i][2] = "DOMAIN:"+ QString::number(domainCount++) +":" + rpn[i][2];
                                    }
                            }
                        }
                    }
                }
            }

        }

        if ( found){
            std::set<QString> domainItems;
            checkIndexItem(domainCount, rpn, copy_rpn,index,domainItems);
            if ( domainItems.size() > 0){
                INamedIdDomain dom;
                dom.prepare();
                for(QString item : domainItems){
                    dom->addItem(new NamedIdentifier(item));
                }
                _domains[domainCount++] = dom;
            }
        }

    }while(found);

    IDomain dom;
    dom.prepare("code=domain:value");
    check(rpn.size() > 0, TR("invalid syntax"));
    if ( rpn.back()[0] == "iff"){
        dom = findOutDomain(rpn, rpn.back());
    }

    return dom;
}

IDomain CalculatorOperation::findOutDomain(const std::vector<std::vector<QString>>&rpn,const std::vector<QString>& node){
    auto findDomainperItem = [&](const std::vector<std::vector<QString>>&rpn, const QString& currentItem)->IDomain{
        if ( currentItem.indexOf("DOMAIN:") == 0)    {
            check(currentItem.size() >7,TR("invalid syntax"));
            int index = currentItem.mid(7,1).toInt();
            return _domains[index];
        } if (currentItem.indexOf("LINK:") == 0){
            check(currentItem.size() >5,TR("invalid syntax"));
            int nextItem = currentItem.mid(5).toInt() ;
            return findOutDomain(rpn, rpn[nextItem]);
        }
        bool ok;
        currentItem.toDouble(&ok);
        if ( ok){
            IDomain dom;
            dom.prepare("code=domain:value");
            return dom;
        }else {
            if ( currentItem.size() > 0 && currentItem[0] == '@'){
                bool ok;
                int n = currentItem.mid(1).toInt(&ok);
                if ( ok){
                     DataDefinition def = datadef(n);
                     if ( def.isValid())
                         return def.domain();
                }
            }
        }
        return IDomain();
    };
    IDomain dom1= findDomainperItem(rpn,node[1]);
    IDomain dom2 = findDomainperItem(rpn,node[2]);

    if ( dom1.isValid() && !dom2.isValid())
        return dom1;
    if ( dom2.isValid() && !dom1.isValid())
        return dom2;
    if ( dom1.isValid() && dom2.isValid()){
        if ( dom1->isCompatibleWith(dom2.ptr()))
            return dom1;
    }else
        throw ErrorObject(TR("Incomaptible or invalid domain used in the expression"));
    return IDomain();

}

int CalculatorOperation::checkIndexItem(int domainCount, std::vector<std::vector<QString>>& rpn,std::vector<std::vector<QString>>& copy_rpn, int index, std::set<QString>& domainItems){
    auto& item = copy_rpn[index];
    // an token may be a regular item or a link; if is a link we follow it further all string encountered belong to the same domain
    if ( index >= rpn.size())
        throw ErrorObject("Corrupt expression; unexpected token found");
    int nextLink1 = checkItem(domainCount,rpn[index][1], item[1], domainItems);
    if ( nextLink1 >= 0)
        checkIndexItem(domainCount, rpn, copy_rpn, nextLink1, domainItems);
    if ( index >= rpn.size())
        throw ErrorObject("Corrupt expression; unexpected token found");
    int nextLink2 =checkItem(domainCount, rpn[index][2], item[2], domainItems);
    if ( nextLink2 >= 0)
        checkIndexItem(domainCount, rpn, copy_rpn, nextLink2, domainItems);
    return 0;
}

void CalculatorOperation::check(bool ok, const QString &error) const
{
    if (!ok){
        throw ErrorObject(error);
    }
}

IDomain CalculatorOperation::linearize(const QStringList &tokens)
{
    if ( tokens.size() == 0)
        return IDomain();

    std::stack<QString> tokenstack;
    std::vector<std::vector<QString>> result;

    bool ok;
    for(const QString& token : tokens)    {
        check(token.length() >0,TR("invalid syntax"));
        if ( token[0] == '@'){
            check(token.length()>=2,TR("Illegal construct in expression, expected number after @:") + token);
            int index = token.mid(1,1).toInt(&ok);
            check(ok, TR("Illegal construct in expression, expected number after @:") + token);
            if (!check(index)){
                kernel()->issues()->log(TR("Illegal parameter index after @, not enough elements as input:") + token);
                return IDomain();
            }
            tokenstack.push(token);
        }else{
            token.toDouble(&ok);
            if ( ok){
                tokenstack.push(token);
            }else {
                if ( token == "?"){
                    tokenstack.push(sUNDEF);
                }
                else if ( token[0] == '\''){
                    tokenstack.push(token);

                }else {
                    std::vector<QString> evalItem;
                    if ( isOperator(token)){
                        check(tokenstack.size() >= 2,TR("Invalid token encountered; not enough values to use after this token: '") + token + "'");
                        QString v1 = tokenstack.top(); tokenstack.pop();
                        QString v2 = tokenstack.top(); tokenstack.pop();
                        evalItem = {token, v1, v2};

                    }else {
                        evalItem.push_back(token);
                       check(_functions.find(token) != _functions.end(), TR("invalid function token:") + token);
                        int n = _functions[token];
                        for(int i=0; i < n; ++i){
                            check(tokenstack.size()>= 1, TR("Invalid syntax"));
                            QString v = tokenstack.top(); tokenstack.pop();
                            evalItem.push_back(v);
                        }
                    }
                    result.push_back(evalItem);
                    tokenstack.push("LINK:" + QString::number(result.size() - 1));
                }
            }
        }
    }
    if ( result.size() == 0){
        // the tokenstack  contains the last item if expression contains only one single actionable token
        if ( tokenstack.size() == 1 && tokenstack.top().indexOf("LINK:") == -1){
            result.push_back({tokenstack.top()});
        }else
            return IDomain();
    }
    try{
        IDomain outputDomain = collectDomainInfo(result);

        for(std::vector<QString>& calc : result){
            bool start = true;
            Action action;
            for(QString part : calc){
                ParmValue val;
                if ( start && calc.size() > 1)    {
                    action._action = string2action(part);
                    if ( action._action == maUNKNOWN){
                        kernel()->issues()->log(TR("Error in expression. Operator type is not valid/ known or improperly used : '") + part + "\'");
                        return IDomain();
                    }
                    start = false;
                }else{
                    int pindex = iUNDEF;
                    check(part.size() > 0, TR("Illegal syntax"));
                    if ( part.indexOf("DOMAIN:") == 0){
                        val._type = CalculatorOperation::DOMAINITEM;
                        // retrieve the domain index in _domains to find the matching domain
                        check(part.size()>6, TR("Invalid syntax"));
                        int domainIndex = part.mid(7,1).toInt();
                        IItemDomain dom = _domains[domainIndex].as<ItemDomain<DomainItem>>();
                        // get the name from the string to retrieve the corresponding domainitem
                        check(part.size()>8, TR("Invalid syntax"));
                        QString itemName =part.mid(9);
                        itemName = itemName.mid(1,itemName.size() - 2);
                        SPDomainItem item = dom->item(itemName);
                        // the raw is stored as this is used to compare against
                        val._value = !item.isNull() ?  item->raw() : rUNDEF;
                    }
                    else if ( part[0] == '\''){
                        val._type = CalculatorOperation::STRING;
                        val._string =part.mid(1,part.size() - 2);
                    }
                    else if ( part[0] == '@'){
                        pindex = part.mid(1,1).toInt(&ok);
                        fillValues(pindex, part, val);
                    }else {
                        double number = part.toDouble(&ok);
                        if (ok){
                            val._type = CalculatorOperation::NUMERIC;
                            val._value = number;
                        }else if (part.indexOf("LINK:") == 0){
                            int link = part.mid(5).toInt();
                            val._type = CalculatorOperation::LINK;
                            val._link = link;
                        }else if ( part == sUNDEF){
                            val._type = CalculatorOperation::NUMERIC;
                            val._value = rUNDEF;
                        }else {
                            kernel()->issues()->log(TR("Error in expression. Index value is not valid :") + part);
                            return IDomain();
                        }
                    }
                    action._values.push_back(val);
                }
            }
            std::reverse(action._values.begin(), action._values.end());
            _actions.push_back(action);
        }
        return outputDomain;
    } catch (ErrorObject& err){
        return IDomain();
    }
}

double CalculatorOperation::calc() {
    auto GetValue = [&](const ParmValue& parm,const std::vector<double>& result, bool *isNumeric=0)->double{
        switch(parm._type){
        case ParmType::LINK:
            return result[parm._link];break;
        case ParmType::ITERATOR:
            return *(*(parm._source));break;
        case ParmType::NUMERIC:{
            if ( isNumeric ) *isNumeric=true;
            return parm._value;break;
        }
        case ParmType::DOMAINITEM:
            return parm._value;break;
        case ParmType::COLUMN:
            return parm._columnValues[_record].toDouble();break;
        }
        return rUNDEF;
    };

    auto CalcBinary = [](MathAction act, double v1, double v2) ->double{
        if (isNumericalUndef(v1)  || isNumericalUndef(v2))
            return rUNDEF;
        switch(act){
        case maADD:
            return v1+v2;
        case maDIVIDE:
            return v2 == 0 ? rUNDEF : v1/v2;
        case maMULT:
            return v1 * v2;
        case maMINUS:
            return v1 - v2;
        case maPOW:
            return std::pow(v1,v2);
        case maMAX:
            return std::max(v1,v2);
        case maMIN:
            return std::min(v1,v2);


        default:
            return rUNDEF;
        }
        return rUNDEF;
    };

    double calcResult;
    std::vector<double> result(_actions.size(),rUNDEF);
    for(int i=0; i < _actions.size(); ++i){
        Action& action = _actions[i];
        switch(action._action){
        case maADD:
        case maMULT:
        case maDIVIDE:
        case maMAX:
        case maMIN:
        case maMINUS:
        case maPOW:
        {
            double v1 = GetValue(action._values[0],result);
            double v2 = GetValue(action._values[1],result);
            calcResult = CalcBinary(action._action,v1,v2);
            break;
        }
        case maSIN:
        {
            double v = GetValue(action._values[0],result);
            calcResult =  isNumericalUndef(v) ? rUNDEF : std::sin(v);
            break;
        }
        case maCOS:
        {
            double v = GetValue(action._values[0],result);
            calcResult =  isNumericalUndef(v) ? rUNDEF : std::cos(v);
            break;
        }
        case maTAN:
        {
            double v = GetValue(action._values[0],result);
            calcResult =  (std::abs(v) == M_PI / 2 || isNumericalUndef(v)) ? rUNDEF : std::tan(v);
            break;
        }
        case maACOS:
        {
            double v = GetValue(action._values[0],result);
            calcResult =  ( v < -1 || v > 1 || isNumericalUndef(v)) ? rUNDEF : std::acos(v);
            break;
        }
        case maASIN:
        {
            double v = GetValue(action._values[0],result);
            calcResult =  ( v < -1 || v > 1 || isNumericalUndef(v)) ? rUNDEF : std::asin(v);
            break;
        }
        case maATAN:
        {
            double v = GetValue(action._values[0],result);
            calcResult =  ( v < -M_PI/2 || v > M_PI/2 || isNumericalUndef(v)) ? rUNDEF : std::acos(v);
            break;
        }
        case maLOG10:
        {
            double v = GetValue(action._values[0],result);
            calcResult =  ( v <= 0 && isNumericalUndef(v)) ? rUNDEF : std::log10(v);
            break;
        }
        case maLN:
        {
            double v = GetValue(action._values[0],result);
            calcResult =  ( v <= 0 && isNumericalUndef(v)) ? rUNDEF : std::log(v);
            break;
        }
        case maABS:
        {
            double v = GetValue(action._values[0],result);
            calcResult =  (isNumericalUndef(v)) ? rUNDEF : std::abs(v);
            break;
        }
        case maSQRT:
        {
            double v = GetValue(action._values[0],result);
            calcResult =  (v < 0 || isNumericalUndef(v)) ? rUNDEF : std::sqrt(v);
            break;
        }
        case maFLOOR:
        {
            double v = GetValue(action._values[0],result);
            calcResult =  isNumericalUndef(v) ? rUNDEF : std::floor(v);
            break;
        }
        case maCEIL:
        {
            double v = GetValue(action._values[0],result);
            calcResult =  isNumericalUndef(v) ? rUNDEF : std::ceil(v);
            break;
        }
        case maEQ:
        {
            bool isNumeric=false;
            double v1 = GetValue(action._values[0],result,&isNumeric);
            double v2 = GetValue(action._values[1],result,&isNumeric);
            calcResult = ( !isNumeric && (isNumericalUndef(v1) || isNumericalUndef(v2))) ? rUNDEF : v1 == v2;
            break;
        }
        case maNEQ:
        {
            bool isNumeric=false;
            double v1 = GetValue(action._values[0],result,&isNumeric);
            double v2 = GetValue(action._values[1],result,&isNumeric);
            calcResult = ( !isNumeric && (isNumericalUndef(v1) || isNumericalUndef(v2))) ? rUNDEF : v1 != v2;
            break;
        }
        case maLESSEQ:
        {
            double v1 = GetValue(action._values[0],result);
            double v2 = GetValue(action._values[1],result);
            calcResult =  isNumericalUndef(v1) || isNumericalUndef(v2) ? rUNDEF : ( v1 <= v2);
            break;
        }
        case maLESS:
        {
            double v1 = GetValue(action._values[0],result);
            double v2 = GetValue(action._values[1],result);
            calcResult = isNumericalUndef(v1) || isNumericalUndef(v2) ? rUNDEF : ( v1 < v2);
            break;
        }
        case maGREATEREQ:
        {
            double v1 = GetValue(action._values[0],result);
            double v2 = GetValue(action._values[1],result);
            calcResult =  isNumericalUndef(v1) ||isNumericalUndef(v2) ? rUNDEF : ( v1 >= v2);
            break;
        }
        case maGREATER:
        {
            double v1 = GetValue(action._values[0],result);
            double v2 = GetValue(action._values[1],result);
            calcResult =  isNumericalUndef(v1) || isNumericalUndef(v2) ? rUNDEF : ( v1 > v2);
            break;
        }
        case maAND:
        {
            double v1 = GetValue(action._values[0],result);
            double v2 = GetValue(action._values[1],result);
            calcResult = isNumericalUndef(v1) || isNumericalUndef(v2) ? rUNDEF : ( (bool)v1 && (bool)v2);
            break;
        }
        case maOR:
        {
            double v1 = GetValue(action._values[0],result);
            double v2 = GetValue(action._values[1],result);
            calcResult =  isNumericalUndef(v1) || isNumericalUndef(v2) ? rUNDEF : ( (bool)v1 || (bool)v2);
            break;
        }
        case maIFF:
        {
            // if the action is a iterator we can directly get its value from the iterator else if it is a
            // comparisson it will be calculated previously and its result will be in calcresult
            if ( action._values[0]._type == CalculatorOperation::ITERATOR)
                calcResult = GetValue(action._values[0],result);
            double v2 = GetValue(action._values[1],result);
            double v3 = GetValue(action._values[2],result);
            calcResult = calcResult == rUNDEF ? rUNDEF : ((bool)calcResult ? v2 : v3);
            break;
        }

        }
        result[i] = calcResult;
    }
    return result.back();
}
