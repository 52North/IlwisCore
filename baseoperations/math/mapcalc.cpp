#include <functional>
#include <future>
#include <QThread>
#include <QCoreApplication>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "numericoperation.h"
#include "domainitem.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "itemdomain.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "MapCalc.h"

using namespace Ilwis;
using namespace BaseOperations;

MapCalc::MapCalc()
{
}

MapCalc::MapCalc(quint64 metaid,const Ilwis::OperationExpression &expr) : NumericOperation(metaid, expr)
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
    _operators["&&"] = { 0, LEFT_ASSOC };
    _operators["||"] = { 0, LEFT_ASSOC };
}

bool MapCalc::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    auto GetValue = [](const ParmValue& parm,const std::vector<double>& result)->double{
        switch(parm._type){
        case ParmType::LINK:
            return result[parm._link];break;
        case ParmType::ITERATOR:
            return *(*(parm._iter));break;
        case ParmType::NUMERIC:
        case ParmType::DOMAINITEM:
            return parm._value;break;
        }
        return rUNDEF;
    };

    auto CalcBinary = [](MathAction act, double v1, double v2) ->double{
        if (isNumericalUndef(v1)  || isNumericalUndef(2))
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

    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    PixelIterator iterOut(_outputRaster);
    std::vector<double> result(_actions.size(),rUNDEF);

    PixelIterator iterEnd = end(iterOut);
    while(iterOut != iterEnd) {
        double calcResult;
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
                calcResult =  v == rUNDEF ? rUNDEF : std::sin(v);
                break;
            }
            case maCOS:
            {
                double v = GetValue(action._values[0],result);
                calcResult =  v == rUNDEF ? rUNDEF : std::cos(v);
                break;
            }
            case maTAN:
            {
                double v = GetValue(action._values[0],result);
                calcResult =  (std::abs(v) == M_PI / 2 || v == rUNDEF) ? rUNDEF : std::tan(v);
                break;
            }
            case maACOS:
            {
                double v = GetValue(action._values[0],result);
                calcResult =  ( v < -1 || v > 1 || v == rUNDEF) ? rUNDEF : std::acos(v);
                break;
            }
            case maASIN:
            {
                double v = GetValue(action._values[0],result);
                calcResult =  ( v < -1 || v > 1 || v == rUNDEF) ? rUNDEF : std::asin(v);
                break;
            }
            case maATAN:
            {
                double v = GetValue(action._values[0],result);
                calcResult =  ( v < -M_PI/2 || v > M_PI/2 || v == rUNDEF) ? rUNDEF : std::acos(v);
                break;
            }
            case maLOG10:
            {
                double v = GetValue(action._values[0],result);
                calcResult =  ( v <= 0 && v == rUNDEF) ? rUNDEF : std::log10(v);
                break;
            }
            case maLN:
            {
                double v = GetValue(action._values[0],result);
                calcResult =  ( v <= 0 && v == rUNDEF) ? rUNDEF : std::log(v);
                break;
            }
            case maABS:
            {
                double v = GetValue(action._values[0],result);
                calcResult =  (v == rUNDEF) ? rUNDEF : std::abs(v);
                break;
            }
            case maSQRT:
            {
                double v = GetValue(action._values[0],result);
                calcResult =  (v < 0 || v == rUNDEF) ? rUNDEF : std::sqrt(v);
                break;
            }
            case maFLOOR:
            {
                double v = GetValue(action._values[0],result);
                calcResult =  v == rUNDEF ? rUNDEF : std::floor(v);
                break;
            }
            case maCEIL:
            {
                double v = GetValue(action._values[0],result);
                calcResult =  v == rUNDEF ? rUNDEF : std::ceil(v);
                break;
            }
            case maEQ:
            {
                double v1 = GetValue(action._values[0],result);
                double v2 = GetValue(action._values[1],result);
                calcResult =  v1 == rUNDEF || v2 == rUNDEF ? 0 : ( v1 == v2);
                break;
            }
            case maNEQ:
            {
                double v1 = GetValue(action._values[0],result);
                double v2 = GetValue(action._values[1],result);
                calcResult =  v1 == rUNDEF || v2 == rUNDEF ? 0 : ( v1 != v2);
                break;
            }
            case maLESSEQ:
            {
                double v1 = GetValue(action._values[0],result);
                double v2 = GetValue(action._values[1],result);
                calcResult =  v1 == rUNDEF || v2 == rUNDEF ? 0 : ( v1 <= v2);
                break;
            }
            case maLESS:
            {
                double v1 = GetValue(action._values[0],result);
                double v2 = GetValue(action._values[1],result);
                calcResult =  v1 == rUNDEF || v2 == rUNDEF ? 0 : ( v1 < v2);
                break;
            }
            case maGREATEREQ:
            {
                double v1 = GetValue(action._values[0],result);
                double v2 = GetValue(action._values[1],result);
                calcResult =  v1 == rUNDEF || v2 == rUNDEF ? 0 : ( v1 >= v2);
                break;
            }
            case maGREATER:
            {
                double v1 = GetValue(action._values[0],result);
                double v2 = GetValue(action._values[1],result);
                calcResult =  v1 == rUNDEF || v2 == rUNDEF ? 0 : ( v1 > v2);
                break;
            }
            case maAND:
            {
                double v1 = GetValue(action._values[0],result);
                double v2 = GetValue(action._values[1],result);
                calcResult =  v1 == rUNDEF || v2 == rUNDEF ? 0 : ( (bool)v1 && (bool)v2);
                break;
            }
            case maOR:
            {
                double v1 = GetValue(action._values[0],result);
                double v2 = GetValue(action._values[1],result);
                calcResult =  v1 == rUNDEF || v2 == rUNDEF ? 0 : ( (bool)v1 || (bool)v2);
                break;
            }
            case maIFF:
            {
                double v1 = GetValue(action._values[0],result);
                double v2 = GetValue(action._values[1],result);
                double v3 = GetValue(action._values[2],result);
                calcResult =  (bool)v1 ? v2 : v3;
                break;
            }

            }
            result[i] = calcResult;
        }
        double v = result.back();
        *iterOut = v;
        ++iterOut;
        for(auto& item : _inputRasters){
            ++(item.second);
        }
    }


    if ( _outputRaster->datadef().domain()->ilwisType() == itNUMERICDOMAIN){
        double rmin = rUNDEF, rmax= rUNDEF;
        bool isInt = true;
        for(double v : _outputRaster ){
            rmin = Ilwis::min(rmin, v);
            rmax = Ilwis::max(rmax, v);
            if ( v != rUNDEF){
                isInt &=  std::abs((qint64)v - v) <EPS8;
            }
        }
        NumericRange *range = new NumericRange(rmin,rmax, isInt ? 1 : 0);
        _outputRaster->datadefRef().range(range);
    }else {
        IFlatTable tbl;
        tbl.prepare();
        tbl->addColumn(_outputRaster->primaryKey(),_outputRaster->datadef().domain());
        int rec = 0;
        ItemRangeIterator iter(_outputRaster->datadef().range<>().data());
        while (iter.isValid()) {
            SPDomainItem item = (*iter);
            tbl->setCell(0,rec++,item->raw());
            ++iter;
        }
        _outputRaster->setAttributes(tbl);

    }

    QVariant value;
    value.setValue<IRasterCoverage>(_outputRaster);
    _outputRaster->setDescription(_expression.toString());
    ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER,_outputRaster->resource() );

    return true;
}

QStringList MapCalc::tokenizer(const QString &expr)
{
    QStringList tokens;
    std::vector<QString> seperators1 = {"*", "+","-","/","(",")","<",">",","};
    std::vector<QString> seperators2 = {"==","!=",">=","<=","&&","||"};
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

QStringList MapCalc::shuntingYard(const QString &expr)
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
            if(tokenstack.empty()){
                kernel()->issues()->log(TR("Illegal construct in expression; maybe missing brackets?"));
                return QStringList();
            }
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

bool MapCalc::isFunction(const QString& func){
    auto iter = _functions.find(func.toLower());
    return iter != _functions.end();
}

bool MapCalc::isNumber(const QString &token) const
{
    bool ok = false;
    token.toUInt(&ok);
    return ok;
}

bool MapCalc::isOperator(const QString& token)
{
    auto iter = _operators.find(token);
    return iter != _operators.end();
}

bool MapCalc::isAssociative(const QString& token, int type)
{
    if (!isOperator(token))
    {
        throw new std::logic_error("Invalid token: " + token.toStdString());
    }
    if (_operators[token][1] == type) {
        return true;
    }
    return false;
}

int MapCalc::cmpPrecedence(const QString& token1, const QString& token2)
{
    if (!isOperator(token1) || !isOperator(token2))
    {
        throw new std::logic_error("Invalid token: " + token1.toStdString() +" " + token2.toStdString());
    }
    return _operators[token1][0] - _operators[token2][0];
}

MapCalc::MathAction MapCalc::string2action(const QString& action){
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
    if ( action == "&&") return maAND;
    if ( action == "||") return maOR;

    return maUNKNOWN;
}

IDomain MapCalc::linearize(const QStringList &tokens)
{
    if ( tokens.size() == 0)
        return IDomain();

    std::stack<QString> tokenstack;
    std::vector<std::vector<QString>> result;

    bool ok;
    for(const QString& token : tokens)    {
        if ( token[0] == '@'){
            if (token.length()<2){
                kernel()->issues()->log(TR("Illegal construct in expression, expected number after @:") + token);
                return IDomain();
            }
            int index = token.mid(1,1).toInt(&ok);
            if (!ok){
                kernel()->issues()->log(TR("Illegal construct in expression, expected number after @:") + token);
                return IDomain();
            }
            if ( index > _inputRasters.size() ){
                kernel()->issues()->log(TR("Illegal parameter index after @, not enough rasters as input:") + token);
                return IDomain();
            }
            auto iterP = _inputRasters.find(index);
            if ( iterP == _inputRasters.end()){
                kernel()->issues()->log(TR("Error in expression. Index for raster parameter not correct :") + token);
                return IDomain();
            }
            tokenstack.push(token);
        }else{
            token.toDouble(&ok);
            if ( ok){
                tokenstack.push(token);
            }else {
                if ( token[0] == '\''){
                    tokenstack.push(token);

                }else {
                    std::vector<QString> evalItem;
                    if ( isOperator(token)){
                        if ( tokenstack.size() < 2){
                            kernel()->issues()->log(TR("Invalid token encountered; not enough values to use after this token: '") + token + "'");
                            return IDomain();
                        }
                        QString v1 = tokenstack.top(); tokenstack.pop();
                        QString v2 = tokenstack.top(); tokenstack.pop();
                        evalItem = {token, v1, v2};

                    }else {
                        evalItem.push_back(token);
                        int n = _functions[token];
                        for(int i=0; i < n; ++i){
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
                    if ( part.indexOf("DOMAIN:") == 0){
                        val._type = MapCalc::DOMAINITEM;
                        // retrieve the domain index in _domains to find the matching domain
                        int domainIndex = part.mid(7,1).toInt();
                        IItemDomain dom = _domains[domainIndex].as<ItemDomain<DomainItem>>();
                        // get the name from the string to retrieve the corresponding domainitem
                        QString itemName =part.mid(9);
                        itemName = itemName.mid(1,itemName.size() - 2);
                        SPDomainItem item = dom->item(itemName);
                        // the raw is stored as this is used to compare against
                        val._value = !item.isNull() ?  item->raw() : rUNDEF;
                    }
                    else if ( part[0] == '\''){
                        val._type = MapCalc::STRING;
                        val._string =part.mid(1,part.size() - 2);
                    }
                    else if ( part[0] == '@'){
                        pindex = part.mid(1,1).toInt(&ok);
                        auto iterP = _inputRasters.find(pindex);
                        val._type = MapCalc::ITERATOR;
                        val._iter = &(*iterP).second;
                        if ( part.size() > 2 ){
                            if(part[2] == '[' && part.endsWith("]")){
                                val._string = part.mid(3,part.size() - 4) ;
                            }
                        }
                    }else {
                        double number = part.toDouble(&ok);
                        if (ok){
                            val._type = MapCalc::NUMERIC;
                            val._value = number;
                        }else if (part.indexOf("LINK:") == 0){
                            int link = part.mid(5).toInt();
                            val._type = MapCalc::LINK;
                            val._link = link;
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

int  checkItem(int domainCount, QString& item, QString& copy_item, std::set<QString>& domainItems){
    if (copy_item.indexOf("LINK:") == -1){
        if ( copy_item[0] == '\''){
            // add a prefix to the string to be able to link it to a domain.
            domainItems.insert(item.mid(1,item.size() - 2));
            item = "DOMAIN:"+ QString::number(domainCount)+":" + item;
        }
        return -1;
    }else {
        int nextLink = copy_item.mid(5).toInt();
        // remove the link as we dont want to encounter when we do a subsequent run through the list of tokens
        copy_item = sUNDEF;
        return nextLink;
    }
}

int checkIndexItem(int domainCount, std::vector<std::vector<QString>>& rpn,std::vector<std::vector<QString>>& copy_rpn, int index, std::set<QString>& domainItems){
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
IDomain MapCalc::collectDomainInfo(std::vector<std::vector<QString>>& rpn){


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
            if ( copy_item[0] == "iff"){
                QString cItem = copy_item[1];
                if (cItem.indexOf("LINK:") == 0){
                    maxLink = std::max(cItem.mid(5).toInt(), maxLink);
                    index = i;
                    found = true;
                }
            }else if ( item[0] == "=="){ // @1='sometext' or 'sometext=@1'
                if ( item[1][0] == '\''){
                    if ( item[2][0] == '@'){
                        IRasterCoverage raster = _inputRasters[item[2].mid(1).toInt()].raster();
                        if ( raster->datadef().domain()->ilwisType() == itITEMDOMAIN){
                            _domains[domainCount] = raster->datadef().domain();
                            rpn[i][1] = "DOMAIN:"+ QString::number(domainCount++) +":" + rpn[i][1];
                        }
                    }
                }else  if ( item[2][0] == '\''){
                    if ( item[1][0] == '@'){
                        IRasterCoverage raster = _inputRasters[item[1].mid(1).toInt()].raster();
                        if ( raster->datadef().domain()->ilwisType() == itITEMDOMAIN){
                            _domains[domainCount] = raster->datadef().domain();
                            rpn[i][2] = "DOMAIN:"+ QString::number(domainCount++) +":" + rpn[i][2];
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
    if ( rpn.back()[0] == "iff"){
        dom = findOutDomain(rpn, rpn.back());
    }

    return dom;

}

IDomain MapCalc::findOutDomain(const std::vector<std::vector<QString>>&rpn,const std::vector<QString>& node){
    auto findDomainperItem = [&](const std::vector<std::vector<QString>>&rpn, const QString& currentItem)->IDomain{
        if ( currentItem.indexOf("DOMAIN:") == 0)    {
            int index = currentItem.mid(7,1).toInt();
            return _domains[index];
        } if (currentItem.indexOf("LINK:") == 0){
            int nextItem = currentItem.mid(5).toInt() ;
            return findOutDomain(rpn, rpn[nextItem]);
        }
        bool ok;
        currentItem.toDouble(&ok);
        if ( ok){
            IDomain dom;
            dom.prepare("code=domain:value");
            return dom;
        }
        return IDomain();
    };
    IDomain dom = findDomainperItem(rpn,node[2]);
    if (!dom.isValid()){
        dom = findDomainperItem(rpn, node[3]);
    }
    return dom;

}

OperationImplementation::State MapCalc::prepare(ExecutionContext *,const SymbolTable&) {


    QString expr = _expression.input<QString>(0);


    for(int parmIndex = 1 ; parmIndex < _expression.parameterCount(); ++parmIndex){
        Parameter parm = _expression.parm(parmIndex);
        if ( hasType(parm.valuetype(), itRASTER)){
            QString url = parm.value();
            IRasterCoverage raster;
            if(!raster.prepare(url)){
                return sPREPAREFAILED;
            }
            _inputRasters[parmIndex] = PixelIterator(raster);
        }else if ( hasType(parm.valuetype(), itNUMBER)){
            bool ok;
            double v = parm.value().toDouble(&ok);
            if (!ok){
                return sPREPAREFAILED;
            }
            _inputNumbers[parmIndex] = v;
        }
    }
    OperationHelperRaster helper;
    helper.initialize((*_inputRasters.begin()).second.raster(), _outputRaster, itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);



    IDomain outputDomain = linearize(shuntingYard(expr));
    if( !outputDomain.isValid())
        return sPREPAREFAILED;

    _outputRaster->datadefRef().domain(outputDomain);

    for(quint32 i = 0; i < _outputRaster->size().zsize(); ++i){
        QString index = _outputRaster->stackDefinition().index(i);
        _outputRaster->setBandDefinition(index,DataDefinition(outputDomain));
    }

    return sPREPARED;
}
//----------------------------------------------------------------------------
REGISTER_OPERATION(MapCalc1)
quint64 MapCalc1::createMetadata()
{
    OperationResource operation({"ilwis://operations/mapcalc"});
    operation.setLongName("MapCalc 1");
    operation.setSyntax("mapcalc(expression,gridcoverage|number)");
    operation.setDescription(TR("Generates a new numerical rastercoverage based on the operation, applied to all the pixels"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itSTRING, TR("Expression"), TR("The expression is an abstract expression were the numbers indicate indexes in the parameter list"));
    operation.addInParameter(1,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("raster coverage"));
    operation.setKeywords("raster,numeric,math");

    mastercatalog()->addItems({operation});
    return operation.id();

}

OperationImplementation *MapCalc1::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new MapCalc1( metaid, expr);
}

MapCalc1::MapCalc1(quint64 metaid,const Ilwis::OperationExpression &expr) : MapCalc(metaid, expr)
{}
//---------------------------------------------------------
REGISTER_OPERATION(MapCalc2)
quint64 MapCalc2::createMetadata()
{
    OperationResource operation({"ilwis://operations/mapcalc"});
    operation.setLongName("MapCalc 2");
    operation.setSyntax("mapcalc(expression,gridcoverage|number,gridcoverage|number)");
    operation.setDescription(TR("Generates a new numerical rastercoverage based on the operation, applied to all the pixels"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itSTRING, TR("Expression"), TR("The expression is an abstract expression were the numbers indicate indexes in the parameter list"));
    operation.addInParameter(1,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(2,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("raster coverage"));
    operation.setKeywords("raster,numeric,math");

    mastercatalog()->addItems({operation});
    return operation.id();

}

OperationImplementation *MapCalc2::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new MapCalc2( metaid, expr);
}

MapCalc2::MapCalc2(quint64 metaid,const Ilwis::OperationExpression &expr) : MapCalc(metaid, expr)
{}
//---------------------------------------------------------
REGISTER_OPERATION(MapCalc3)
quint64 MapCalc3::createMetadata()
{
    OperationResource operation({"ilwis://operations/mapcalc"});
    operation.setLongName("MapCalc 3");
    operation.setSyntax("mapcalc(expression,gridcoverage|number,gridcoverage|number,gridcoverage|number)");
    operation.setDescription(TR("Generates a new numerical rastercoverage based on the operation, applied to all the pixels, max 3 parameters"));
    operation.setInParameterCount({4});
    operation.addInParameter(0,itSTRING, TR("Expression"), TR("The expression is an abstract expression were the numbers indicate indexes in the parameter list"));
    operation.addInParameter(1,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(2,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(3,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("raster coverage"));
    operation.setKeywords("raster,numeric,math");

    mastercatalog()->addItems({operation});
    return operation.id();

}

OperationImplementation *MapCalc3::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new MapCalc3( metaid, expr);
}

MapCalc3::MapCalc3(quint64 metaid,const Ilwis::OperationExpression &expr) : MapCalc(metaid, expr)
{}
//---------------------------------------------------------
REGISTER_OPERATION(MapCalc4)
quint64 MapCalc4::createMetadata()
{
    OperationResource operation({"ilwis://operations/mapcalc"});
    operation.setLongName("MapCalc 4");
    operation.setSyntax("mapcalc(expression,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number)");
    operation.setDescription(TR("Generates a new numerical rastercoverage based on the operation, applied to all the pixels, max 4 parameters"));
    operation.setInParameterCount({5});
    operation.addInParameter(0,itSTRING, TR("Expression"), TR("The expression is an abstract expression were the numbers indicate indexes in the parameter list"));
    operation.addInParameter(1,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(2,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(3,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(4,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("raster coverage"));
    operation.setKeywords("raster,numeric,math");
    mastercatalog()->addItems({operation});
    return operation.id();

}

OperationImplementation *MapCalc4::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new MapCalc4( metaid, expr);
}

MapCalc4::MapCalc4(quint64 metaid,const Ilwis::OperationExpression &expr) : MapCalc(metaid, expr)
{}

//---------------------------------------------------------
REGISTER_OPERATION(MapCalc5)
quint64 MapCalc5::createMetadata()
{
    OperationResource operation({"ilwis://operations/mapcalc"});
    operation.setLongName("MapCalc 5");
    operation.setSyntax("mapcalc(expression,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number)");
    operation.setDescription(TR("Generates a new numerical rastercoverage based on the operation, applied to all the pixels, max 5 parameters"));
    operation.setInParameterCount({6});
    operation.addInParameter(0,itSTRING, TR("Expression"), TR("The expression is an abstract expression were the numbers indicate indexes in the parameter list"));
    operation.addInParameter(1,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(2,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(3,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(4,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(5,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("raster coverage"));
    operation.setKeywords("raster,numeric,math");

    mastercatalog()->addItems({operation});
    return operation.id();

}

OperationImplementation *MapCalc5::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new MapCalc5( metaid, expr);
}

MapCalc5::MapCalc5(quint64 metaid,const Ilwis::OperationExpression &expr) : MapCalc(metaid, expr)
{}

//---------------------------------------------------------
REGISTER_OPERATION(MapCalc6)
quint64 MapCalc6::createMetadata()
{
    OperationResource operation({"ilwis://operations/mapcalc"});
    operation.setLongName("MapCalc 6");
    operation.setSyntax("mapcalc(expression,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number)");
    operation.setDescription(TR("Generates a new numerical rastercoverage based on the operation, applied to all the pixels, max 6 parameters"));
    operation.setInParameterCount({6});
    operation.addInParameter(0,itSTRING, TR("Expression"), TR("The expression is an abstract expression were the numbers indicate indexes in the parameter list"));
    operation.addInParameter(1,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(2,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(3,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(4,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(5,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(6,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("raster coverage"));
    operation.setKeywords("raster,numeric,math");

    mastercatalog()->addItems({operation});
    return operation.id();

}

OperationImplementation *MapCalc6::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new MapCalc6( metaid, expr);
}

MapCalc6::MapCalc6(quint64 metaid,const Ilwis::OperationExpression &expr) : MapCalc(metaid, expr)
{}
//---------------------------------------------------------
REGISTER_OPERATION(MapCalc7)
quint64 MapCalc7::createMetadata()
{
    OperationResource operation({"ilwis://operations/mapcalc"});
    operation.setLongName("MapCalc 7");
    operation.setSyntax("mapcalc(expression,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number)");
    operation.setDescription(TR("Generates a new numerical rastercoverage based on the operation, applied to all the pixels, max 7 parameters"));
    operation.setInParameterCount({6});
    operation.addInParameter(0,itSTRING, TR("Expression"), TR("The expression is an abstract expression were the numbers indicate indexes in the parameter list"));
    operation.addInParameter(1,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(2,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(3,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(4,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(5,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(6,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(7,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("raster coverage"));
    operation.setKeywords("raster,numeric,math");

    mastercatalog()->addItems({operation});
    return operation.id();

}

OperationImplementation *MapCalc7::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new MapCalc7( metaid, expr);
}

MapCalc7::MapCalc7(quint64 metaid,const Ilwis::OperationExpression &expr) : MapCalc(metaid, expr)
{}
//---------------------------------------------------------
REGISTER_OPERATION(MapCalc8)
quint64 MapCalc8::createMetadata()
{
    OperationResource operation({"ilwis://operations/mapcalc"});
    operation.setLongName("MapCalc 8");
    operation.setSyntax("mapcalc(expression,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number)");
    operation.setDescription(TR("Generates a new numerical rastercoverage based on the operation, applied to all the pixels, max 8 parameters"));
    operation.setInParameterCount({6});
    operation.addInParameter(0,itSTRING, TR("Expression"), TR("The expression is an abstract expression were the numbers indicate indexes in the parameter list"));
    operation.addInParameter(1,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(2,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(3,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(4,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(5,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(6,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(7,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(8,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("raster coverage"));
    operation.setKeywords("raster,numeric,math");

    mastercatalog()->addItems({operation});
    return operation.id();

}

OperationImplementation *MapCalc8::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new MapCalc8( metaid, expr);
}

MapCalc8::MapCalc8(quint64 metaid,const Ilwis::OperationExpression &expr) : MapCalc(metaid, expr)
{}

//---------------------------------------------------------
REGISTER_OPERATION(MapCalc9)
quint64 MapCalc9::createMetadata()
{
    OperationResource operation({"ilwis://operations/mapcalc"});
    operation.setLongName("MapCalc 9");
    operation.setSyntax("mapcalc(expression,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number)");
    operation.setDescription(TR("Generates a new numerical rastercoverage based on the operation, applied to all the pixels, max 9 parameters"));
    operation.setInParameterCount({6});
    operation.addInParameter(0,itSTRING, TR("Expression"), TR("The expression is an abstract expression were the numbers indicate indexes in the parameter list"));
    operation.addInParameter(1,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(2,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(3,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(4,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(5,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(6,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(7,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(8,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(9,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("raster coverage"));
    operation.setKeywords("raster,numeric,math");

    mastercatalog()->addItems({operation});
    return operation.id();

}

OperationImplementation *MapCalc9::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new MapCalc9( metaid, expr);
}

MapCalc9::MapCalc9(quint64 metaid,const Ilwis::OperationExpression &expr) : MapCalc(metaid, expr)
{}
