#include "raster.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "mastercatalog.h"
#include "setattributetabel.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(SetAttributeTable)

SetAttributeTable::SetAttributeTable()
{
}


SetAttributeTable::SetAttributeTable(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

bool SetAttributeTable::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    _outputRaster->primaryKey(_primaryKey);
    _outputRaster->setAttributes(_inputTable);

    PixelIterator iterOut(_outputRaster);
    for(auto v : _inputRaster){
        *iterOut = v;
        ++iterOut;
    }


    QVariant v;
    v.setValue(_outputRaster);
    logOperation(_outputRaster, _expression);
    ctx->setOutput(symTable, v, _outputRaster->name(),itRASTER, _outputRaster->resource());

    return true;
}

Ilwis::OperationImplementation *SetAttributeTable::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SetAttributeTable(metaid, expr);
}

Ilwis::OperationImplementation::State SetAttributeTable::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);

    OperationHelper::check([&] ()->bool { return _inputTable.prepare(_expression.input<QString>(0), itTABLE); },
    {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" } );

    OperationHelper::check([&] ()->bool { return _inputRaster.prepare(_expression.input<QString>(1), itRASTER); },
    {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(1), "" } );

    _primaryKey = _expression.input<QString>(2);

    int index = _inputTable->columnIndex(_primaryKey);
    if ( index == iUNDEF){
        kernel()->issues()->log(TR("Table doesnt contain column ") + _primaryKey);
        return sPREPAREFAILED;
    }
    std::vector<QVariant> values = _inputTable->column(_primaryKey);
    std::set<QVariant> uniques;
    int undefs = 0;
    for(QVariant v : values){
        if ( isNumericalUndef(v.toDouble()))
            ++undefs;
        else
            uniques.insert(v);
    }
    if ( (uniques.size() + undefs ) != values.size()){
        kernel()->issues()->log(TR("Key column must not contain duplicate values"));
        return sPREPAREFAILED;
    }

    ColumnDefinition coldef = _inputTable->columndefinition(index);
    if ( !coldef.datadef().domain()->isCompatibleWith(_inputRaster->datadef().domain().ptr())){
        kernel()->issues()->log(TR("Key column doesnt match raster domain ") + _primaryKey);
        return sPREPAREFAILED;
    }

    OperationHelperRaster helper;
    helper.initialize(_inputRaster, _outputRaster, itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF|itDOMAIN);

    for(quint32 i = 0; i < _outputRaster->size().zsize(); ++i){
        QString index = _outputRaster->stackDefinition().index(i);
        DataDefinition datadef = _outputRaster->datadef(i);
        _outputRaster->setBandDefinition(index,DataDefinition(datadef.domain(), datadef.range()->clone()));
    }
    return sPREPARED;
}

quint64 SetAttributeTable::createMetadata()
{
    OperationResource operation({"ilwis://operations/setattributetable"});
    operation.setSyntax("setattributetable(inputtable, raster, primarykey)");
    operation.setDescription(TR("assigns the input table as attribute data to coverage, note that for features is is a (kind of) copy as they dont have real attribute tables"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itTABLE , TR("input table"),TR("Table with at least one column suitable as primary key, For features this column must hold the indexes for the features to be coupled"));
    operation.addInParameter(1,itRASTER , TR("coverage"),TR("For rasters the domain must be the same as the primary key, features are coupled either by index or by index number in the key column"));
    operation.addInParameter(2,itSTRING , TR("key column"),TR("Column that serves as primary key for connecting the table to the coverage"), OperationResource::ueCOMBO);
    operation.addValidation(0,2,"columns with domain=numericdomain,textdomain,itemdomain");
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER , TR("output raster"),TR("A new raster with an attribute table"));
    operation.setKeywords("coverage, selection");

    mastercatalog()->addItems({operation});
    return operation.id();

}
