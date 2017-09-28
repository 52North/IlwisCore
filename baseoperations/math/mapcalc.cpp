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
        updateTranquilizer(iterOut.linearPosition(), 1000);
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
    logOperation(_outputRaster, _expression);
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

DataDefinition MapCalc::datadef(int index)
{
    auto iterP = _inputRasters.find(index);
    if ( iterP != _inputRasters.end()){
        IRasterCoverage raster = _inputRasters[index].raster();
        return raster->datadef();
    }
    return DataDefinition();
}



OperationImplementation::State MapCalc::prepare(ExecutionContext *ctx,const SymbolTable &st) {

    OperationImplementation::prepare(ctx,st);
    QString expr = _expression.input<QString>(0);

    RasterStackDefinition stackdef;
    for(int parmIndex = 1 ; parmIndex < _expression.parameterCount(); ++parmIndex){
        Parameter parm = _expression.parm(parmIndex);
        if ( hasType(parm.valuetype(), itRASTER)){
            QString url = parm.value();
            IRasterCoverage raster;
            if(!raster.prepare(url)){
                return sPREPAREFAILED;
            }
            if ( stackdef.isValid()){
                if(!stackdef.checkStackDefintion(raster->stackDefinition())){
                    kernel()->issues()->log(TR("Incompatible stack definition for ") +raster->name() ) ;
                    return sPREPAREFAILED;
                }
            }else if ( raster->stackDefinition().domain()->code().indexOf("count") == -1)
                stackdef = raster->stackDefinition();
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
    if ( stackdef.isValid()){
        _outputRaster->stackDefinitionRef() = stackdef;
    }
    IDomain outputDomain;
    try {
        outputDomain = linearize(shuntingYard(expr));
        if( !outputDomain.isValid())
            return sPREPAREFAILED;
    } catch(ErrorObject& err){
        return sPREPAREFAILED;
    }

    _outputRaster->datadefRef().domain(outputDomain);

    for(quint32 i = 0; i < _outputRaster->size().zsize(); ++i){
        QString index = _outputRaster->stackDefinition().index(i);
        _outputRaster->setBandDefinition(index,DataDefinition(outputDomain));
    }
    initialize(_outputRaster->size().linearSize());

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
//REGISTER_OPERATION(MapCalc7)
//quint64 MapCalc7::createMetadata()
//{
//    OperationResource operation({"ilwis://operations/mapcalc"});
//    operation.setLongName("MapCalc 7");
//    operation.setSyntax("mapcalc(expression,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number)");
//    operation.setDescription(TR("Generates a new numerical rastercoverage based on the operation, applied to all the pixels, max 7 parameters"));
//    operation.setInParameterCount({6});
//    operation.addInParameter(0,itSTRING, TR("Expression"), TR("The expression is an abstract expression were the numbers indicate indexes in the parameter list"));
//    operation.parameterNeedsQuotes(0);
//    operation.addInParameter(1,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.addInParameter(2,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.addInParameter(3,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.addInParameter(4,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.addInParameter(5,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.addInParameter(6,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.addInParameter(7,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.setOutParameterCount({1});
//    operation.addOutParameter(0,itRASTER, TR("raster coverage"));
//    operation.setKeywords("raster,numeric,math");

//    mastercatalog()->addItems({operation});
//    return operation.id();

//}

//OperationImplementation *MapCalc7::create(quint64 metaid, const Ilwis::OperationExpression &expr)
//{
//    return new MapCalc7( metaid, expr);
//}

//MapCalc7::MapCalc7(quint64 metaid,const Ilwis::OperationExpression &expr) : MapCalc(metaid, expr)
//{}
////---------------------------------------------------------
//REGISTER_OPERATION(MapCalc8)
//quint64 MapCalc8::createMetadata()
//{
//    OperationResource operation({"ilwis://operations/mapcalc"});
//    operation.setLongName("MapCalc 8");
//    operation.setSyntax("mapcalc(expression,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number)");
//    operation.setDescription(TR("Generates a new numerical rastercoverage based on the operation, applied to all the pixels, max 8 parameters"));
//    operation.setInParameterCount({6});
//    operation.addInParameter(0,itSTRING, TR("Expression"), TR("The expression is an abstract expression were the numbers indicate indexes in the parameter list"));
//    operation.parameterNeedsQuotes(0);
//    operation.addInParameter(1,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.addInParameter(2,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.addInParameter(3,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.addInParameter(4,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.addInParameter(5,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.addInParameter(6,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.addInParameter(7,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.addInParameter(8,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.setOutParameterCount({1});
//    operation.addOutParameter(0,itRASTER, TR("raster coverage"));
//    operation.setKeywords("raster,numeric,math");

//    mastercatalog()->addItems({operation});
//    return operation.id();

//}

//OperationImplementation *MapCalc8::create(quint64 metaid, const Ilwis::OperationExpression &expr)
//{
//    return new MapCalc8( metaid, expr);
//}

//MapCalc8::MapCalc8(quint64 metaid,const Ilwis::OperationExpression &expr) : MapCalc(metaid, expr)
//{}

////---------------------------------------------------------
//REGISTER_OPERATION(MapCalc9)
//quint64 MapCalc9::createMetadata()
//{
//    OperationResource operation({"ilwis://operations/mapcalc"});
//    operation.setLongName("MapCalc 9");
//    operation.setSyntax("mapcalc(expression,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number,gridcoverage|number)");
//    operation.setDescription(TR("Generates a new numerical rastercoverage based on the operation, applied to all the pixels, max 9 parameters"));
//    operation.setInParameterCount({6});
//    operation.addInParameter(0,itSTRING, TR("Expression"), TR("The expression is an abstract expression were the numbers indicate indexes in the parameter list"));
//    operation.parameterNeedsQuotes(0);
//    operation.addInParameter(1,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.addInParameter(2,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.addInParameter(3,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.addInParameter(4,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.addInParameter(5,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.addInParameter(6,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.addInParameter(7,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.addInParameter(8,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.addInParameter(9,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
//    operation.setOutParameterCount({1});
//    operation.addOutParameter(0,itRASTER, TR("raster coverage"));
//    operation.setKeywords("raster,numeric,math");

//    mastercatalog()->addItems({operation});
//    return operation.id();

//}

//OperationImplementation *MapCalc9::create(quint64 metaid, const Ilwis::OperationExpression &expr)
//{
//    return new MapCalc9( metaid, expr);
//}

//MapCalc9::MapCalc9(quint64 metaid,const Ilwis::OperationExpression &expr) : MapCalc(metaid, expr)
//{}
