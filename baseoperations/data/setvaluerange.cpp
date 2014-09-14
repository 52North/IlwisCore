#include "raster.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "pixeliterator.h"
#include "mastercatalog.h"
#include "setvaluerange.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(SetValueRange)

SetValueRange::SetValueRange()
{
}

SetValueRange::SetValueRange(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool SetValueRange::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    DataDefinition& datadef = _table.isValid() ? _table->columndefinition(_columnName).datadef() : _raster->datadefRef();
    const SPNumericRange numrange = datadef.range<NumericRange>();
    const SPNumericRange rngDomain = datadef.domain()->range<NumericRange>();
    double lmin = _min == rUNDEF ? numrange->min() : std::max(_min, rngDomain->min());
    double lmax = _max == rUNDEF ? numrange->max() : std::min(_max, rngDomain->max());
    double lstep = _step == rUNDEF ? numrange->resolution() : std::max(_step, rngDomain->resolution());
    bool defaultMinmaxUsed = rngDomain->min() == -1.00E+15 || rngDomain->min() == -1.00E+300;
    NumericRange *rng = new NumericRange(lmin, lmax,lstep);
    datadef.range(rng);
    datadef.domain()->range(rng);

    if ( _table.isValid()) {
        std::vector<QVariant> data = _table->column(_columnName);
        for(auto& val : data) {
            val = rng->ensure(val);
            if (!defaultMinmaxUsed && lstep != 0){
                val = rngDomain->min() + lstep * ((qint64)(val.toDouble() - rngDomain->min()) / lstep);
            }
        }
        _table->column(_columnName, data);
    } else {
        std::function<bool(const BoundingBox)> SetValrange = [&](const BoundingBox box ) -> bool {
            PixelIterator iter(_raster, box);

            std::for_each(iter, iter.end(), [&](double& val){
                val = rng->ensure(val).value<double>();
                if (!defaultMinmaxUsed && lstep != 0){
                    val = rngDomain->min() + lstep * ((qint64)(val - rngDomain->min()) / lstep);
                }
            });
            return true;
        };
        return OperationHelperRaster::execute(ctx, SetValrange, _raster);
    }

    return true;
}

Ilwis::OperationImplementation *SetValueRange::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SetValueRange(metaid, expr);
}

Ilwis::OperationImplementation::State SetValueRange::prepare(ExecutionContext *, const SymbolTable &)
{
    QString objectName = _expression.parm(0).value();
    int index = 0;
    if ( (index = objectName.indexOf("{"))!= -1) {
        _columnName = objectName.mid(index + 1);
        _columnName = _columnName.left(_columnName.size() - 2);
        objectName = objectName.left(index);
    }
    IFeatureCoverage features;
    if ( _columnName != "") {
        if(!features.prepare(objectName)) {
            if(!_table.prepare(objectName)) {
                if ( !_raster.prepare(objectName)) {
                    ERROR2(ERR_COULD_NOT_LOAD_2, "object", objectName);
                    return sPREPAREFAILED;
                }
            }
        }
        if ( features.isValid())
            _table = features->attributeTable();
        else if ( _raster.isValid() && !_raster->attributeTable().isValid()) {
            _table = _raster->attributeTable();
        }
        if ( _table.isValid()) {
            if ( _table->columnIndex(_columnName) == iUNDEF) {
                ERROR2(ERR_NOT_FOUND2, _columnName, objectName);
                return sPREPAREFAILED;
            }
            if (hasType(!_table->columndefinition(_columnName).datadef().domain<>()->valueType(), itNUMBER)) {
                ERROR2(ERR_OPERATION_NOTSUPPORTED2,TR("operation"), TR("non numeric data"));
            }
        }  else {
            if (hasType(!_raster->attributeTable()->columndefinition(_columnName).datadef().domain<>()->valueType(), itNUMBER)) {
                ERROR2(ERR_OPERATION_NOTSUPPORTED2,TR("operation"), TR("non numeric data"));
            }
        }
    } else{
        if ( !_raster.prepare(objectName)) {
            ERROR2(ERR_COULD_NOT_LOAD_2, "raster", objectName);
            return sPREPAREFAILED;
        }
    }
    if ( (_table.isValid() && _table->isReadOnly()) ||
         (_raster.isValid() && _raster->isReadOnly())){
        WARN1(WARN_READ_ONLY1,objectName);
        return sPREPAREFAILED;

    }

    QString minTxt = _expression.parm(1).value();
    QString maxTxt = _expression.parm(2).value();

    bool ok = true;
    if ( minTxt.trimmed() != ""){
        _min = minTxt.toDouble(&ok);
    }
    if ( maxTxt.trimmed() != ""){
        _max = maxTxt.toDouble(&ok);
    }
    if ( _expression.parameterCount() == 4) {
        QString stepTxt = _expression.parm(3).value();
        if ( stepTxt.trimmed() != ""){
            _step = stepTxt.toDouble(&ok);
        }
    }
    if ( !ok) {
        ERROR2(ERR_ILLEGAL_VALUE_2, TR("parameter"),TR("expression"));
        return sPREPAREFAILED;
    }
    return sPREPARED;
}

quint64 SetValueRange::createMetadata()
{
    QString url = QString("ilwis://operations/setvaluerange");
    Resource resource(QUrl(url), itOPERATIONMETADATA);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","setvaluerange");
    resource.addProperty("syntax","setvaluerange(objectname,min,max(,resolution))");
    resource.addProperty("description",TR("sets the value range of a coverage or column to a new value; all value outside the range will become undefined"));
    resource.addProperty("inparameters","3|4");
    resource.addProperty("pin_1_type", itSTRING);
    resource.addProperty("pin_1_name", TR("input object"));
    resource.addProperty("pin_1_desc",TR("input object. If the syntax uses the [] notation it points to a column of a table"));
    resource.addProperty("pin_2_type", itNUMBER);
    resource.addProperty("pin_2_name", TR("minimum"));
    resource.addProperty("pin_2_desc",TR("minimum of the new value range. If the value is undefined it will be ignored"));
    resource.addProperty("pin_3_type", itNUMBER);
    resource.addProperty("pin_3_name", TR("maximum"));
    resource.addProperty("pin_3_desc",TR("maximum of the new value range. If the value is undefined it will be ignored"));
    resource.addProperty("pin_4_type", itNUMBER);
    resource.addProperty("pin_4_name", TR("resolution"));
    resource.addProperty("pin_4_desc",TR("optional resolution of the new value range. If the value is undefined it will be ignored"));
    resource.addProperty("outparameters",0);

    resource.prepare();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();
}
