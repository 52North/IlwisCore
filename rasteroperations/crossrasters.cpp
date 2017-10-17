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
#include "tranquilizer.h"
#include "crossrasters.h"
#include <unordered_map>

using namespace Ilwis;
using namespace RasterOperations;

CrossRastersBase::CrossRastersBase()
{
}

CrossRastersBase::CrossRastersBase(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

#define SHIFTER 10e8
const unsigned int MAGIC_NUMBER = 300000;

struct Combo {
    Combo(quint32 id=iUNDEF, quint32 count=0) : _id(id), _count(count) {}
    quint32 _id;
    quint32 _count;
};

QString CrossRastersBase::determineCrossId(double v1, double v2) const{
    QString elem1 = _inputRaster1->datadef().domain<>()->impliedValue(v1).toString();
    QString elem2 = _inputRaster2->datadef().domain<>()->impliedValue(v2).toString();
    QString id = QString("%1 * %2").arg(elem1 == "" ? "?" : elem1).arg(elem2 == "" ? "?" :elem2);
    switch (_undefhandling){
    case uhIgnoreUndef:
        if (v1 == rUNDEF || v2 == rUNDEF)
            id = "";
        break;
    case uhIgnoreUndef1:
        if (v1 == rUNDEF)
            id = "";
        break;
    case uhIgnoreUndef2:
        if (v2 == rUNDEF)
            id = "";
        break;
    default:
        break;
    }
    return id;
}

void CrossRastersBase::checkUndef(double& v1, double& v2){
    if ( isNumericalUndef2(v1,_inputRaster1) )
        v1 = rUNDEF;
    if ( isNumericalUndef2(v2,_inputRaster2) )
        v2 = rUNDEF;
}

bool CrossRastersBase::crossWithRaster(const  BoundingBox& box){
    PixelIterator iterIn1(_inputRaster1, box);
    PixelIterator iterIn2(_inputRaster2, box);
    PixelIterator iterOut(_outputRaster, box);
    quint32 idcount = 0;
    initialize(_outputRaster->size().linearSize());
    quint32 count = 0;
    double pixarea = _inputRaster1->georeference()->pixelSize();
    std::map<quint64, Combo> combos;
    std::vector<quint64> inCreationOrder;
    std::for_each(iterIn1, iterIn1.end(), [&](double& v1){
        double v2 = *iterIn2;

        checkUndef(v1,v2);
        bool ignore = (_undefhandling == uhIgnoreUndef ) && (v1 == rUNDEF || v1 == rUNDEF);

        if (!ignore) {
            quint64 combo = v1 + v2 * SHIFTER;
            auto iterCombos = combos.find(combo);
            if ( iterCombos == combos.end()){
                combos[combo]._count = 1;
                combos[combo]._id = idcount;
                *iterOut = idcount;
                inCreationOrder.push_back(combo);
                ++idcount;
            }
            else{
                (*(iterCombos)).second._count++;
                *iterOut = (*(iterCombos)).second._id;
            }
        }else
            *iterOut = rUNDEF;

        updateTranquilizer(++count,100);
        ++iterIn2;
        ++iterOut;
    });
    quint32 record = 0;
    NamedIdentifierRange *idrange = new NamedIdentifierRange();
    count = 0;
    trq()->prepare(_metadata->name(),TR("Updating table"), combos.size());
    for(quint64 orderedCombo : inCreationOrder) {
        auto element = combos[orderedCombo];
        quint32 v2 = orderedCombo / SHIFTER;
        quint32 v1 = orderedCombo  - v2 * SHIFTER;

        QString id = determineCrossId(v1,v2);
        if (id != ""){
            *idrange << id;
            _outputTable->setCell(0,record,QVariant(record));
            _outputTable->setCell(1,record,QVariant(v1));
            _outputTable->setCell(2,record,QVariant(v2));
            _outputTable->setCell(3,record,QVariant(element._count))          ;
            _outputTable->setCell(4,record,QVariant(element._count * pixarea)) ;
            _outputTable->setCell(5,record,QVariant(record)); // coverage key
            ++record;
        }
        updateTranquilizer(count++,10);
    }
    _crossDomain->range(idrange);

    return true;
}
struct ComboValues {
    ComboValues(double v1,double  v2) : _v1(v1), _v2(v2){}
    bool operator==(const ComboValues& cb) const{
        return _v1 == cb._v1 && _v2 == cb._v2;
    }

    double _v1;
    double _v2;
};

struct KeyHasher
{
    std::size_t operator()(const ComboValues& k) const
    {
        std::size_t h1 = std::hash<double>()(k._v1);
        std::size_t h2 = std::hash<double>()(k._v2);
        std::size_t h3 = h1 ^(h2 >> 1);
        return h3;
    }
};

bool CrossRastersBase::crossNoRaster( const BoundingBox& box){
    PixelIterator iterIn1(_inputRaster1, box);
    PixelIterator iterIn2(_inputRaster2, box);
    std::unordered_map<ComboValues, quint64,KeyHasher> combos;
    quint32 count = 0;
    double pixarea = _inputRaster1->georeference()->pixelSize();
    pixarea *= pixarea;

    auto end = iterIn1.end();
    while(iterIn1 != end){
        double v1 = *iterIn1;
        double v2 = *iterIn2;

        checkUndef(v1,v2);
        ComboValues combo(v1,v2);
        auto iterCombos = combos.find(combo);
        if ( iterCombos == combos.end())
            combos[combo] = 1;
        else
            (*(iterCombos)).second++;

        ++iterIn2;
        ++iterIn1;

        updateTranquilizer(count++,1000);
    }
    if ( combos.size() > 50000){
        kernel()->issues()->log(TR("Number of unique combinations exceeds the maximum 50000 ( = ") + QString::number(combos.size() ) + " )");
        return false;

    }
    quint32 record = 0;
    NamedIdentifierRange *idrange = new NamedIdentifierRange();
    count = 0;
    trq()->prepare(_metadata->name(),TR("Updating table"), combos.size());
    for(auto element : combos) {
        ComboValues combo = element.first;
        double v2 = combo._v2;
        double v1 = combo._v1;
        QString id = determineCrossId(v1,v2);
        if ( id != "") {
            *idrange << id;
            _outputTable->setCell(0,record,QVariant(record));
            _outputTable->setCell(1,record,QVariant(v1));
            _outputTable->setCell(2,record,QVariant(v2));
            _outputTable->setCell(3,record,QVariant(element.second));
            double totalArea = element.second * pixarea;
            _outputTable->setCell(4,record,QVariant(totalArea))  ;
            ++record;
        }
        updateTranquilizer(count++,10);
    }
    _crossDomain->range(idrange);

    return true;
}

bool CrossRastersBase::execute(ExecutionContext *ctx, SymbolTable &symTable)
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
        logOperation(_outputTable,_expression);
        ctx->setOutput(symTable,value,_outputTable->name(), itTABLE, _outputTable->resource() );
        if ( _outputRaster.isValid()) {
            QVariant outraster;
            outraster.setValue<IRasterCoverage>(_outputRaster);
            ctx->addOutput(symTable,outraster,_outputRaster->name(), itRASTER, _outputRaster->resource() );
        }
    }
    return ok;
}

Ilwis::OperationImplementation *CrossRastersBase::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CrossRastersBase(metaid, expr);
}

Ilwis::OperationImplementation::State CrossRastersBase::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
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
        newTable.prepare(QString(INTERNAL_CATALOG + "/%1").arg(outputName));
    else
        newTable.prepare();

    QString crossName = QString("%1_%2").arg(_inputRaster1->name(), _inputRaster2->name());
    crossName.replace(".","_");
    _crossDomain.prepare();
    _crossDomain->name(crossName);
    _crossDomain->range(new NamedIdentifierRange());


    QString undefh = _expression.parm(2).value().toLower();
    _undefhandling = uhDontCare;
    if ( undefh == "ignoreundef1" )
        _undefhandling = uhIgnoreUndef1;
    else if ( undefh == "ignoreundef2" )
        _undefhandling = uhIgnoreUndef2;
    else if ( undefh == "ignoreundef" )
        _undefhandling = uhIgnoreUndef;


    newTable->addColumn("cross_combinations", _crossDomain);
    newTable->addColumn("first_raster", _inputRaster1->datadef().domain<>());
    newTable->addColumn("second_raster", _inputRaster2->datadef().domain<>());
    newTable->addColumn("pixel_count", IlwisObject::create<IDomain>("count"));
    newTable->addColumn("pixel_area", IlwisObject::create<IDomain>("value"));
    _outputTable = newTable;


    initialize(_inputRaster1->size().linearSize());

    return sPREPARED;
}

quint64 CrossRastersBase::createMetadata(OperationResource& operation)
{
    operation.setInParameterCount({3});
    operation.addInParameter(0,itRASTER , TR("first rastercoverage"),TR("input rastercoverage with domain item or integer"));
    operation.addInParameter(1,itRASTER , TR("second rastercoverage"),TR("input rastercoverage with domain item or integer"));
    operation.addInParameter(2,itSTRING , TR("undef handling"),TR("how undefs are handled can be defined per input raster"));
    operation.setKeywords("cross,raster,table");

    mastercatalog()->addItems({operation});
    return operation.id();

}

//-------------------------------------------------------------------------------
REGISTER_OPERATION(CrossRasters)

CrossRasters::CrossRasters(quint64 metaid, const Ilwis::OperationExpression &expr) : CrossRastersBase(metaid, expr)
{}

Ilwis::OperationImplementation *CrossRasters::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CrossRasters(metaid,expr);
}

quint64 CrossRasters::createMetadata()
{

    OperationResource operation({"ilwis://operations/cross"});
    operation.setLongName("Cross");
    operation.setSyntax("cross(raster1, raster2, undefhandling=!ignoreundef|ignoreundef1 | ignoreundef2 | dontcare)");
    operation.setDescription(TR("Performs an overlay of two raster maps. Pixels values combinations on the same location are stored and aggregate combination info is stored in a table"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itTABLE, TR("output table"),TR("output table with the results of the cross operation"));
    operation.setKeywords("cross,raster,table");
    return CrossRastersBase::createMetadata(operation) ;

}

//------------------------------------------------------------------
REGISTER_OPERATION(CrossRastersWithRasterOutput)

CrossRastersWithRasterOutput::CrossRastersWithRasterOutput(quint64 metaid, const Ilwis::OperationExpression &expr)
{}

Ilwis::OperationImplementation *CrossRastersWithRasterOutput::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CrossRastersWithRasterOutput(metaid, expr);
}

Ilwis::OperationImplementation::State CrossRastersWithRasterOutput::prepare(ExecutionContext *ctx, const SymbolTable &symTable)
{
    if ( CrossRastersBase::prepare(ctx, symTable) == sPREPAREFAILED)
        return sPREPAREFAILED;
    OperationHelperRaster helper;
    helper.initialize(_inputRaster1, _outputRaster, itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);
    if ( _outputRaster.isValid()) {
        DataDefinition def(_crossDomain);
        _outputRaster->datadefRef() = def;
        for ( int band = 0; band < _outputRaster->size().zsize(); ++band){
            _outputRaster->datadefRef(band) = def;
        }
        _outputTable->addColumn(_outputRaster->primaryKey(), _crossDomain);
        _outputRaster->setAttributes(_outputTable);
        return sPREPARED;
    }
    return sPREPAREFAILED;
}

quint64 CrossRastersWithRasterOutput::createMetadata()
{

    OperationResource operation({"ilwis://operations/crosswithraster"});
    operation.setLongName("Cross with raster output");
    operation.setSyntax("crosswithraster(raster1, raster2, undefhandling=!ignoreundef|ignoreundef1 | ignoreundef2 | dontcare)");
    operation.setDescription(TR("Performs an overlay of two raster maps. Pixels values combinations on the same location are stored and aggregate combination info is stored in a table. A raster is generated which shows the combinations"));

    operation.setOutParameterCount({2});
    operation.addOutParameter(0,itTABLE, TR("output table"),TR("output table with the results of the cross operation"));
    operation.addOutParameter(1,itRASTER, TR("output raster"),TR("output raster with the results of the cross operation"));

    return CrossRastersBase::createMetadata(operation) ;
}
