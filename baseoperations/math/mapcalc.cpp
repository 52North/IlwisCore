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
#include "calculatoroperation.h"
#include "mapcalc.h"

using namespace Ilwis;
using namespace BaseOperations;

MapCalc::MapCalc()
{
}

MapCalc::MapCalc(quint64 metaid,const Ilwis::OperationExpression &expr) : CalculatorOperation(metaid, expr)
{

}

bool MapCalc::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
     if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    PixelIterator iterOut(_outputRaster);

    PixelIterator iterEnd = end(iterOut);
    while(iterOut != iterEnd) {
        double v = calc();
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
    _outputRaster->addDescription(_expression.toString());
    ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER,_outputRaster->resource() );

    return true;
}

bool MapCalc::check(int index) const {
    auto iter = _inputRasters.find(index);
    if ( iter == _inputRasters.end() ){
        auto iter2 = _inputNumbers.find(index);
        if ( iter2 == _inputNumbers.end()){
            return false;
        }
    }
    return true;
}

void MapCalc::fillValues(int pindex,const QString& part, ParmValue& val)  {
    auto iterP = _inputRasters.find(pindex);
    if ( iterP != _inputRasters.end()){
        val._type = MapCalc::ITERATOR;
        val._source = &(*iterP).second;
        if ( part.size() > 2 ){
            if(part[2] == '[' && part.endsWith("]")){
                val._string = part.mid(3,part.size() - 4) ;
            }
        }
    }else {
         auto iterP = _inputNumbers.find(pindex);
         val._type = MapCalc::NUMERIC;
         val._value = (*iterP).second;
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
                        int index = item[2].mid(1).toInt();
                        auto iterP = _inputRasters.find(index);
                        if ( iterP != _inputRasters.end()){
                            IRasterCoverage raster = _inputRasters[index].raster();
                            if ( raster->datadef().domain()->ilwisType() == itITEMDOMAIN){
                                _domains[domainCount] = raster->datadef().domain();
                                rpn[i][1] = "DOMAIN:"+ QString::number(domainCount++) +":" + rpn[i][1];
                            }
                        }
                    }
                }else  if ( item[2][0] == '\''){
                    if ( item[1][0] == '@'){
                        int index = item[2].mid(1).toInt();
                        auto iterP = _inputRasters.find(index);
                        if ( iterP != _inputRasters.end()){
                            IRasterCoverage raster = _inputRasters[index].raster();
                            if ( raster->datadef().domain()->ilwisType() == itITEMDOMAIN){
                                _domains[domainCount] = raster->datadef().domain();
                                rpn[i][2] = "DOMAIN:"+ QString::number(domainCount++) +":" + rpn[i][2];
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
    operation.parameterNeedsQuotes(0);
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
    operation.parameterNeedsQuotes(0);
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
    operation.parameterNeedsQuotes(0);
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
    operation.parameterNeedsQuotes(0);
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
    operation.parameterNeedsQuotes(0);
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
    operation.parameterNeedsQuotes(0);
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
    operation.parameterNeedsQuotes(0);
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
    operation.parameterNeedsQuotes(0);
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
    operation.parameterNeedsQuotes(0);
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
