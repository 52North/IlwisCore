#include <functional>
#include <future>
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "domain.h"
#include "numericdomain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "coverage.h"
#include "rastercoverage.h"
#include "featurecoverage.h"
#include "operationhelper.h"
#include "pixeliterator.h"
#include "featureiterator.h"
#include "feature.h"
#include "operationhelpergrid.h"
#include "operationhelperfeatures.h"
#include "symboltable.h"
#include "internaldatabaseconnection.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "numericoperation.h"
#include "tranquilizer.h"
#include "columnjoin.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(ColumnJoin)

ColumnJoin::ColumnJoin()
{

}

ColumnJoin::ColumnJoin(quint64 metaid, const Ilwis::OperationExpression &expr) : NumericOperation(metaid, expr)
{

}

OperationImplementation *ColumnJoin::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new ColumnJoin(metaid, expr);
}

bool ColumnJoin::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    boost::container::flat_map<QString, int> columns;
    for(int i=0; i < _outputTable->columnCount(); ++i){
        columns[_outputTable->columndefinition(i).name()] = i;
    }
    int offset = _baseTable->columnCount();
    std::vector<QVariant> basevalues = _baseTable->column(_primaryKeyColumn);
    std::vector<QVariant> foreignvalues = _foreignTable->column(_foreignKeyColumn);
    boost::container::flat_map<QVariant, int> mapping;
    boost::container::flat_map<int, int> recordMapping;
    for(int record = 0; record < basevalues.size(); ++record){
        mapping[basevalues[record]] = record;
    }
    for(int record = 0; record < foreignvalues.size(); ++record){
        auto iter = mapping.find(foreignvalues[record]);
        if ( iter != mapping.end()){
            recordMapping[(*iter).second] = record;
        }
    }
    for(int i=0; i < recordMapping.size(); ++i)
        _outputTable->newRecord();

    int record = 0;
    for(auto recMapping: recordMapping){
        Record recBase = _baseTable->record(recMapping.first);
        for(int c=0; c < recBase.columnCount(); ++c)
            _outputTable->setCell(c,record,recBase.cell(c));
        Record recForeign = _foreignTable->record(recMapping.second);
        for(int c=0; c < recForeign.columnCount(); ++c)
            _outputTable->setCell(offset + c,record,recForeign.cell(c));
        ++record;
    }

    setOutput(_outputTable, ctx, symTable);
    return true;
}

OperationImplementation::State ColumnJoin::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString baseName = _expression.parm(0).value();
    IIlwisObject obj;
    if (!obj.prepare(baseName)) {
        kernel()->issues()->log(QString(TR("%1 is not a valid table").arg(baseName)));
        return sPREPAREFAILED;
    }
    IlwisTypes tp = obj->ilwisType();
    if (tp & itCOVERAGE) {
        _inputCoverage = obj.as<Coverage>();
        _baseTable = _inputCoverage->attributeTable();
        if ( !_baseTable.isValid()){
            kernel()->issues()->log(QString(TR("%1 has no valid attribute table").arg(baseName)));
            return sPREPAREFAILED;
        }
    } else if (tp & itTABLE) {
        _baseTable = obj.as<Table>();
        if ( !_baseTable.isValid()){
            kernel()->issues()->log(QString(TR("%1 is not a valid table").arg(baseName)));
            return sPREPAREFAILED;
        }
    } else {
        kernel()->issues()->log(QString(TR("%1 is not a valid table").arg(baseName)));
        return sPREPAREFAILED;
    }
    _primaryKeyColumn = _expression.parm(1).value();

    QString inputTable = _expression.parm(2).value();
    QUrl url(inputTable);
    if (!_foreignTable.prepare(inputTable,{"mustexist", true})) {
        ERROR2(ERR_COULD_NOT_LOAD_2,inputTable,"");
        return sPREPAREFAILED;
    }

    ColumnDefinition def1 = _baseTable->columndefinition(_primaryKeyColumn);
    if ( !def1.isValid()){
        kernel()->issues()->log(TR("Could not find column in input data:") + _primaryKeyColumn);
        return sPREPAREFAILED;
    }
    _foreignKeyColumn = _expression.parm(3).value();
     ColumnDefinition def2 = _foreignTable->columndefinition(_foreignKeyColumn);

     if ( !def1.datadef().domain()->isCompatibleWith(def2.datadef().domain().ptr())){
        kernel()->issues()->log(TR("Column domains are not compatible:") + def1.datadef().domain()->name() + " and " + def2.datadef().domain()->name()) ;
        return sPREPAREFAILED;
     }

    QString outName = _expression.parm(0, false).value();
    if (baseName != outName) {
        if (outName == sUNDEF){
            IFlatTable  ftable;
            ftable.prepare();
            _outputTable = ftable;
        } else if(!_outputTable.prepare(outName)) {// output table doesnt need to exists
            _outputTable.prepare(QString(INTERNAL_CATALOG + "/%1").arg(outName), _baseTable->ilwisType());
            _outputTable->name(outName);
        }
    } else{
        _outputTable = _baseTable;
    }
    for(int i=0; i < _baseTable->columnCount(); ++i){
        _outputTable->addColumn(_baseTable->columndefinition(i));
    }
    for(int i=0; i < _foreignTable->columnCount(); ++i){
        _outputTable->addColumn(_foreignTable->columndefinition(i));
    }

    if ( _inputCoverage.isValid()){
        if ( _inputCoverage->ilwisType() == itRASTER){
             IRasterCoverage raster = OperationHelperRaster::initialize(_inputCoverage,itRASTER, itDOMAIN|itGEOREF|itCOORDSYSTEM|itRASTERSIZE|itENVELOPE);
             _outputCoverage = raster;
        }else {
            IFeatureCoverage features = OperationHelperFeatures::initialize(_inputCoverage, itFEATURE, itDOMAIN|itCOORDSYSTEM|itENVELOPE);
            _outputCoverage = features;
        }
    }
    return sPREPARED;
}

quint64 ColumnJoin::createMetadata()
{
    OperationResource operation({"ilwis://operations/joinattributes"});
    operation.setLongName("Join Attributes");
    operation.setSyntax("joinattributes(base-table,column-name,input-table, column-name)");
    operation.setDescription(TR("Join a base table or coverage with attributes from another table that share a common domain"));
    operation.setInParameterCount({4});
    operation.addInParameter(0,itTABLE|itCOVERAGE, TR("table/ coverage"),TR("Base table or coverage with attributes from which where join is to do be done"));
    operation.addInParameter(1,itSTRING | itNUMBER , TR("input column name"),TR("column with a numerical domain or number"), OperationResource::ueCOMBO);
    operation.addInParameter(2,itTABLE, TR("input-table"),TR("input table-column from which the input column will be chosen"));
    operation.addInParameter(3,itSTRING | itNUMBER, TR("input column name or number"),TR("column with a numerical domain or number"), OperationResource::ueCOMBO);
    operation.setOutParameterCount({1});
    operation.addValidation(0,1,"columns");
    operation.addValidation(2,3,"columns");
    operation.addOutParameter(0,itTABLE|itCOVERAGE, TR("output table/ coverage"));
    operation.setKeywords("table,aggregate,column");
    mastercatalog()->addItems({operation});
    return operation.id();
}

