#include <functional>
#include <future>
#include <QThread>
#include <QCoreApplication>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "numericoperation.h"
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
    _operators["+"] = { 0, LEFT_ASSOC };
    _operators["-"] = { 0, LEFT_ASSOC };
    _operators["*"] = { 5, LEFT_ASSOC };
    _operators["/"] = { 5, LEFT_ASSOC };
    _operators[">"] = { 0, LEFT_ASSOC };
    _operators["<"] = { 0, LEFT_ASSOC };
    _operators["<="] = { 0, LEFT_ASSOC };
    _operators[">="] = { 0, LEFT_ASSOC };
    _operators["!="] = { 0, LEFT_ASSOC };
    _operators["=="] = { 0, LEFT_ASSOC };
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
            return parm._value;break;
        }
        return rUNDEF;
    };

    auto CalcBinary = [](MathAction act, double v1, double v2) ->double{
        if (v1 == rUNDEF  || v2 == rUNDEF)
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
    double rmin = rUNDEF, rmax= rUNDEF, precission = 0;
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

    QVariant value;
    value.setValue<IRasterCoverage>(_outputRaster);
    ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER,_outputRaster->resource() );

    return true;
}




QStringList MapCalc::shuntingYard(const QString &expr)
{
    QStringList rpn;
    std::stack<QString> tokenstack;

    QStringList parts = expr.split(QRegExp("\\b"));
    QStringList tokens;
    // some post processing on the tokenization; more complex regexp needed
    for(QString part : parts){
        part = part.trimmed();
        if ( part != ""){
            if ( part[0] == ')' && part.size() > 1){
                for(int i=0; i < part.size(); ++i){
                    if ( part[i] != ' ')
                        tokens.push_back(QString(part[i]));
                }

            }else
                tokens.push_back(part);
        }
    }

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
            tokenstack.pop();
            if ( isFunction(tokenstack.top())){
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
    auto iter = _functions.find(func);
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

    return maUNKNOWN;
}

std::vector<std::vector<QString>> MapCalc::linearize(const QStringList &tokens)
{
    std::stack<QString> tokenstack;
    std::vector<std::vector<QString>> result;
    bool ok;
    for(const QString& token : tokens)    {
        token.toInt(&ok);
        if ( ok){
            tokenstack.push(token);
        }else {
            std::vector<QString> evalItem;
            if ( isOperator(token)){
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
            int n = -result.size();
            tokenstack.push(QString::number(n));
        }
    }
    for(std::vector<QString>& calc : result){
        bool start = true;
        Action action;
        for(QString part : calc){
            if ( start)    {
                action._action = string2action(part);
                if ( action._action == maUNKNOWN){
                    kernel()->issues()->log(TR("Error in expression. Operation value is not valid or known :") + part);
                    return std::vector<std::vector<QString>>();
                }
                start = false;
            }else{
                int vindex = part.toInt(&ok);
                if (!ok){
                    kernel()->issues()->log(TR("Error in expression. Index value is not valid :") + part);
                    return std::vector<std::vector<QString>>();
                }
                ParmValue val;
                auto iterP = _inputRasters.find(vindex);
                if ( iterP != _inputRasters.end()){
                    val._type = MapCalc::ITERATOR;
                    val._iter = &(*iterP).second;
                }else {
                    auto iterP = _inputNumbers.find(vindex);
                    if ( iterP != _inputNumbers.end()){
                        val._type = MapCalc::NUMERIC;
                        val._value = (*iterP).second;
                    }else if ( vindex < 0){
                        val._type = MapCalc::LINK;
                        val._link = std::abs(vindex) - 1;
                    }else {
                        kernel()->issues()->log(TR("Error in expression. Index value is not valid :") + part);
                        return std::vector<std::vector<QString>>();
                    }
                }
                action._values.push_back(val);
            }
        }
        std::reverse(action._values.begin(), action._values.end());
        _actions.push_back(action);
    }
    return result;
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

    IDomain dom;
    dom.prepare("code=domain:value");
    _outputRaster->datadefRef().domain(dom);

    for(quint32 i = 0; i < _outputRaster->size().zsize(); ++i){
        QString index = _outputRaster->stackDefinition().index(i);
        _outputRaster->setBandDefinition(index,DataDefinition(dom));
    }

    linearize(shuntingYard(expr));

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
