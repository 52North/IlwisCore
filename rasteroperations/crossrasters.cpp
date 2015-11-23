#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "columndefinition.h"
#include "basetable.h"
#include "flattable.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "crossrasters.h"

using namespace Ilwis;
using namespace RasterOperations;

CrossRasters::CrossRasters()
{
}

CrossRasters::CrossRasters(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

#define SHIFTER 10e8
const unsigned int MAGIC_NUMBER = 300000;

struct Combo {
    Combo(quint32 id=iUNDEF, quint32 count=0) : _id(id), _count(count) {}
    quint32 _id;
    quint32 _count;
};

bool CrossRasters::crossWithRaster(const  BoundingBox& box){
    PixelIterator iterIn1(_inputRaster1, box);
    PixelIterator iterIn2(_inputRaster2, box);
    PixelIterator iterOut(_outputRaster, box);
    quint32 idcount = 0;
    std::map<quint64, Combo> combos;
    std::for_each(iterIn1, iterIn1.end(), [&](double v1){
        double v2 = *iterIn2;
        if ( isNumericalUndef2(v1,_inputRaster1) )
            v1 = MAGIC_NUMBER;
        if ( isNumericalUndef2(v2,_inputRaster2) )
            v2 = MAGIC_NUMBER;

        bool ignore = (_undefhandling == uhIgnoreUndef ) && (v1 == MAGIC_NUMBER || v1 == MAGIC_NUMBER);

        if (!ignore) {
            quint64 combo = v1 + v2 * SHIFTER;
            auto iterCombos = combos.find(combo);
            if ( iterCombos == combos.end()){
                combos[combo]._count = 1;
                combos[combo]._id = idcount;
                *iterOut = idcount;
                ++idcount;
            }
            else{
                (*(iterCombos)).second._count++;
                *iterOut = (*(iterCombos)).second._id;
            }
        }
        ++iterIn2;
        ++iterOut;
    });
    quint32 record = 0;
    NamedIdentifierRange *idrange = new NamedIdentifierRange();
    for(auto element : combos) {
        quint64 combo = element.first;
        quint32 v2 = combo / SHIFTER;
        quint32 v1 = combo  - v2 * SHIFTER;
        QString elem1 = _inputRaster1->datadef().domain<>()->impliedValue(v1).toString();
        QString elem2 = _inputRaster2->datadef().domain<>()->impliedValue(v2).toString();
        QString id = QString("%1 * %2").arg(elem1).arg(elem2);;
        switch (_undefhandling){
        case uhIgnoreUndef:
            if (v1 == MAGIC_NUMBER || v1 == MAGIC_NUMBER)
                id = "";
            break;
        case uhIgnoreUndef1:
            if (v1 == MAGIC_NUMBER)
                id = elem2;
            break;
        case uhIgnoreUndef2:
            if (v2 == MAGIC_NUMBER)
                id = elem1;
            break;
        default:
            break;
        }
        if (id != ""){
            idrange->add(new NamedIdentifier(id, element.second._id ));
            _outputTable->setCell(0,record,QVariant(record));
            _outputTable->setCell(1,record,QVariant(v1));
            _outputTable->setCell(2,record,QVariant(v2));
            Combo& cm = element.second;
            _outputTable->setCell(3,record,QVariant(cm._count))          ;
            ++record;
        }
    }
    _crossDomain->range(idrange);

    return true;
}

bool CrossRasters::crossNoRaster( const BoundingBox& box){
    PixelIterator iterIn1(_inputRaster1, box);
    PixelIterator iterIn2(_inputRaster2, box);
    std::map<quint64, quint64> combos;
    std::for_each(iterIn1, iterIn1.end(), [&](double& v1){
        qint32 v2 = *iterIn2;
        quint64 combo = v1 + v2 * SHIFTER;
        auto iterCombos = combos.find(combo);
        if ( iterCombos == combos.end())
            combos[combo] = 1;
        else
            (*(iterCombos)).second++;
        ++iterIn2;
    });
    quint32 record = 0;
    NamedIdentifierRange *idrange = new NamedIdentifierRange();
    for(auto element : combos) {
        quint64 combo = element.first;
        quint32 v2 = combo / SHIFTER;
        quint32 v1 = combo  - v2 * SHIFTER;
        QString id = QString("%1 * %2").arg(_inputRaster1->datadef().domain<>()->impliedValue(v1).toString()).
                arg(_inputRaster2->datadef().domain<>()->impliedValue(v2).toString());
        *idrange << id;
        _outputTable->setCell(0,record,QVariant(record));
        _outputTable->setCell(1,record,QVariant(v1));
        _outputTable->setCell(2,record,QVariant(v2));
        Combo cm = element.second;
        _outputTable->setCell(3,record,QVariant(cm._count))        ;
        ++record;
    }
    _crossDomain->range(idrange);

    return true;
}

bool CrossRasters::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    if ( !_inputRaster1->georeference()->isCompatible(_inputRaster2->georeference())) {
        if (!OperationHelperRaster::resample(_inputRaster1, _inputRaster2, ctx)) {
            return ERROR2(ERR_COULD_NOT_CONVERT_2, TR("georeferences"), TR("common base"));
        }
    }

    bool ok;
    if ( _outputRaster.isValid())
        ok = crossWithRaster(_inputRaster1->size());
    else
        ok = crossNoRaster(_inputRaster1->size());

    if ( ok && ctx != 0) {
        QVariant value;
        value.setValue<ITable>(_outputTable);
        ctx->setOutput(symTable,value,_outputTable->name(), itTABLE, _outputTable->source() );
        if ( _outputRaster.isValid()) {
            QVariant outraster;
            outraster.setValue<IRasterCoverage>(_outputRaster);
            ctx->addOutput(symTable,outraster,_outputRaster->name(), itRASTER, _outputRaster->source() );
        }
    }
    return ok;
}

Ilwis::OperationImplementation *CrossRasters::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CrossRasters(metaid, expr);
}

Ilwis::OperationImplementation::State CrossRasters::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    QString raster1 = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

    if (!_inputRaster1.prepare(raster1, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster1,"");
        return sPREPAREFAILED;
    }

    QString raster2 = _expression.parm(1).value();

    if (!_inputRaster2.prepare(raster2, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster2,"");
        return sPREPAREFAILED;
    }

    IFlatTable newTable;
    if ( outputName != sUNDEF)
        newTable.prepare(QString("ilwis://internalcatalog/%1").arg(outputName));
    else
        newTable.prepare();

    QString crossName = QString("%1_%2").arg(raster1, raster2);
    crossName.replace(".","_");
    _crossDomain.prepare();
    _crossDomain->name(crossName);
    _crossDomain->range(new NamedIdentifierRange());

    if ( _expression.parameterCount(false) == 2) {
        outputName = _expression.parm(1,false).value();
        OperationHelperRaster helper;
        helper.initialize(_inputRaster1, _outputRaster, itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);
        if ( _outputRaster.isValid()) {
            _outputRaster->datadefRef().domain(_crossDomain);
            _outputRaster->name(outputName);
        }

    }
    QString undefh = _expression.parm(2).value().toLower();
    _undefhandling = uhDontCare;
    if ( undefh == "ignoreundef1" )
        _undefhandling = uhIgnoreUndef1;
    else if ( undefh == "ignoreundef2" )
        _undefhandling = uhIgnoreUndef2;
    else if ( undefh == "ignoreundef" )
        _undefhandling = uhIgnoreUndef;


    newTable->addColumn(crossName, _crossDomain);
    newTable->addColumn(_inputRaster1->name(), _inputRaster1->datadef().domain<>());
    newTable->addColumn(_inputRaster2->name(), _inputRaster2->datadef().domain<>());
    newTable->addColumn("NPix", IlwisObject::create<IDomain>("count"));
    newTable->addColumn("Area", IlwisObject::create<IDomain>("value"));
    _outputTable = newTable;

    return sPREPARED;
}

quint64 CrossRasters::createMetadata()
{

    OperationResource operation({"ilwis://operations/cross"});
    operation.setSyntax("cross(raster1, raster2, undefhandling=!ignoreundef1 | ignoreundef2 | ignoreundef | dontcare)");
    operation.setDescription(TR("generates a new boolean map based on the logical condition used"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itRASTER , TR("first rastercoverage"),TR("input rastercoverage with domain item or integer"));
    operation.addInParameter(1,itRASTER , TR("second rastercoverage"),TR("input rastercoverage with domain item or integer"));
    operation.addInParameter(2,itSTRING , TR("undef handling"),TR("how undefs are handled can be defined per input raster"));
    operation.setOutParameterCount({1,2});
    operation.addOutParameter(0,itTABLE, TR("output table"),TR("output table with the results of the cross operation"));
    operation.addOptionalOutParameter(1,itRASTER, TR("output raster"),TR("optional output raster with the results of the cross operation"));
    operation.setKeywords("cross,raster,table");

    mastercatalog()->addItems({operation});
    return operation.id();
}
