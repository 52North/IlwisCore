#include <QString>
#include <QStringList>
#include <functional>
#include <future>
#include <memory>
#include "coverage.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "numericdomain.h"
#include "numericrange.h"
#include "juliantime.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "operationhelper.h"
#include "identifierrange.h"
#include "feature.h"
#include "featurecoverage.h"
#include "convertcolumndomain.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(ConvertColumnDomain)

ConvertColumnDomain::ConvertColumnDomain()
{

}

ConvertColumnDomain::ConvertColumnDomain(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

template<class DomainType, class RangeType> bool translate2ItemColumn(ITable& inputTable, const QString& colName, std::vector<QVariant>& values){

    //std::vector<QVariant> values = oldvalues;
    RangeType range;
    IlwisTypes orgValueType = inputTable->columndefinitionRef(colName).datadef().domain()->valueType();
    if ( hasType(orgValueType, itDOMAINITEM )){
        std::set<QString> items;
        IItemDomain itemdom = inputTable->columndefinitionRef(colName).datadef().domain().as<ItemDomain<DomainItem>>();
        for(auto& v : values){
            auto item = itemdom->item(v.toUInt());
            if (item){
                items.insert(item->name());
                v = item->name();
            }else
                v = QVariant();
        }
        for(const auto& v : items){
            range << v;
        }
    }else{

        for(const auto& v : values){
           range << v.toString();
        }
    }
    DomainType domain;

    domain.prepare();
    domain->range(new RangeType(range));
    inputTable->columndefinitionRef(colName) = ColumnDefinition(colName, domain);
    for(auto& v : values){
        auto item = domain->item(v.toString());
        if ( item)
            v = item->raw();
        else{
            kernel()->issues()->log(TR("Non convertible or invalid values in domain. No conversion possible"));
            return false;
        }
    }
    //inputTable->column(colName, values);
    return true;
}

bool translate2TimeColumn(ITable& inputTable, const QString& colName, std::vector<QVariant>& values){
    ITimeDomain domain;
    domain.prepare();
    TimeInterval range;
    for(auto& v : values){
        Time t(v);
        if ( t.isValid()){
            v = (double)t;
            range.add((double)t);
        }
    }
    domain->range(new TimeInterval(range));
    inputTable->columndefinitionRef(colName) = ColumnDefinition(colName, domain);

    return true;
}

template<class NumberType> bool translate2NumericColumn(ITable& inputTable, const QString& colName, std::vector<QVariant>& values, double undef){
    INumericDomain domain;
    domain.prepare();
    NumericRange range;
    for(auto& v : values){
        bool ok;
        NumberType nv = (NumberType) v.toDouble(&ok);
        v = ok ? nv : undef;
        range.add(nv);
    }
    domain->range(new NumericRange(range));
    inputTable->columndefinitionRef(colName) = ColumnDefinition(colName, domain);

    return true;
}

bool translate2colorColumn(ITable& , const QString& , std::vector<QVariant>& ){
// TODO: not yet implemented
    kernel()->issues()->log("Using unimplemeted feature", IssueObject::itWarning);

    return false;
}

bool ConvertColumnDomain::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    bool ok = false;
    std::vector<QVariant> values = _inputTable->column(_columnName);
    if ( _targetDomainType == "identifier" ){
        ok = translate2ItemColumn<INamedIdDomain, NamedIdentifierRange>(_inputTable, _columnName, values);
    }
    if ( _targetDomainType == "thematic" ){
        ok = translate2ItemColumn<IThematicDomain, ThematicRange>(_inputTable, _columnName, values);
    }
    if ( _targetDomainType == "time" ){
        ok = translate2TimeColumn(_inputTable, _columnName, values);
    }
    if ( _targetDomainType == "float" ){
        translate2NumericColumn<double>(_inputTable, _columnName, values, rUNDEF);
    }
    if ( _targetDomainType == "integer" ){
        ok = translate2NumericColumn<qint32>(_inputTable, _columnName, values, iUNDEF);
    }
    if ( _targetDomainType == "color" ){
        ok = translate2colorColumn(_inputTable, _columnName, values);
    }
    if (ok){
       _inputTable->column(_columnName, values);
       _inputTable->columndefinitionRef(_columnName).datadef().domain()->name(_domainName);

        if ( ctx != 0) {
            QVariant value;
            value.setValue<ITable>(_inputTable);
            ctx->setOutput(symTable, value, _inputTable->name(), itTABLE,_inputTable->source());
        }
    }



    return ok;
}

Ilwis::OperationImplementation *ConvertColumnDomain::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new ConvertColumnDomain(metaid, expr);
}

Ilwis::OperationImplementation::State ConvertColumnDomain::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    QString sourceUrl = _expression.input<QString>(0);
    if(!_inputTable.prepare(sourceUrl,{"mustexist", true})){
        IFeatureCoverage fc;
        if(!fc.prepare(sourceUrl,{"mustexist", true})){
            ERROR2(ERR_COULD_NOT_LOAD_2,sourceUrl,"");
            return sPREPAREFAILED;
        }
        _inputTable = fc->attributeTable();
    }
    QString colName = _expression.input<QString>(1);
    if ( _inputTable->columnIndex(colName) == iUNDEF){
       ERROR2(ERR_COLUMN_MISSING_2,colName,_inputTable->name());
       return sPREPAREFAILED;
    }
    _sourceDomainType = _inputTable->columndefinition(colName).datadef().domain()->ilwisType();
    if (  !hasType(_sourceDomainType,itTEXTDOMAIN | itNUMERICDOMAIN | itITEMDOMAIN)){
        ERROR2(ERR_INVALID_PROPERTY_FOR_2,TR("Domain type"),colName);
        return sPREPAREFAILED;
    }
    _columnName = colName;
    QString targetDom = _expression.input<QString>(2).toLower();
    std::vector<QString> types {"identifier","thematic","time","float","integer","color"};
    if ( std::find(types.begin(), types.end(), targetDom) == types.end()){
        ERROR2(ERR_INVALID_PROPERTY_FOR_2,TR("Domain type"),colName);
        return sPREPAREFAILED;
    }
    _targetDomainType = targetDom;
    _domainName = _columnName;
    if ( _expression.parameterCount() == 4){
        _domainName = _expression.input<QString>(3);
    }

    return sPREPARED;
}

quint64 ConvertColumnDomain::createMetadata()
{
    OperationResource operation({"ilwis://operations/convertcolumndomain"});
    operation.setLongName("convert column to other domain");
    operation.setSyntax("convertcolumndomain(inputtable,columnname,targetdomaintype=!identifier|thematic|time|float|integer|color[, domain-name])");
    operation.setDescription(TR("translates the values of a string column in a table to a regular domain"));
    operation.setInParameterCount({3,4});
    operation.addInParameter(0,itTABLE|itFEATURE, TR("input table/coverage"),TR("input table/coverage with a to be translated string/numeric or id domain column"));
    operation.addInParameter(1,itSTRING,  TR("string column"),TR("Column to be translated; must contain string values"));
    operation.addInParameter(2,itSTRING, TR("target domain type"),TR("The domain to which the string values are to be translated") );
    operation.addOptionalInParameter(3,itSTRING,  TR("domain name"),TR("optional name of the to be created domain. If not given it will get the name of the column"));
    operation.setOutParameterCount({0});
    operation.setKeywords("table, domain, transformation");

    mastercatalog()->addItems({operation});
    return operation.id();
}



