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
using namespace RasterOperations;\

CrossRasters::CrossRasters()
{
}

CrossRasters::CrossRasters(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

#define SHIFTER 10e8
bool CrossRasters::cross(const Box3D<qint32> box){
    PixelIterator iterIn1(_inputRaster1, box);
    PixelIterator iterIn2(_inputRaster2, box);
    std::map<quint64, quint64> combos;
    for_each(iterIn1, iterIn1.end(), [&](double& v1){
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
        QString id = QString("%1 * %2").arg(_inputRaster1->datadef().domain()->value(v1)).arg(_inputRaster2->datadef().domain()->value(v2));
        *idrange << id;
        _outputTable->setCell(0,record,QVariant(record));
        _outputTable->setCell(1,record,QVariant(v1));
        _outputTable->setCell(2,record,QVariant(v2));
        _outputTable->setCell(3,record,QVariant(element.second))        ;
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

//    std::function<bool(const Box3D<qint32>)> Cross = [&](const Box3D<qint32> box ) -> bool {
//        PixelIterator iterIn1(_inputRaster1, box);
//        PixelIterator iterIn2(_inputRaster2, box);
//        std::unordered_map<quint64, quint64> combos;
//        for_each(iterIn1, iterIn1.end(), [&](double& v1){
//            qint32 v2 = *iterIn2;
//            quint64 combo = v1 + v2 * 10e8;
//            auto iterCombos = combos.find(combo);
//            if ( iterCombos == combos.end())
//                combos[combo] = 0;
//            else
//                (*(iterCombos)).second++;
//            ++iterIn2;
//        });
//        return true;
//    };

    bool ok = cross(_inputRaster1->size());

    if ( !_inputRaster1->georeference()->isCompatible(_inputRaster2->georeference())) {
        if (!OperationHelperRaster::resample(_inputRaster1, _inputRaster2, ctx)) {
            return ERROR2(ERR_COULD_NOT_CONVERT_2, TR("georeferences"), TR("common base"));
        }
    }

    return true;
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
    newTable.prepare(QString("ilwis://internalcatalog/%1").arg(outputName));
    QString crossName = QString("%1_%2").arg(raster1, raster2);
    crossName.replace(".","_");
    _crossDomain.prepare();
    _crossDomain->setName(crossName);
    _crossDomain->range(new NamedIdentifierRange());


    newTable->addColumn(crossName, _crossDomain);
    newTable->addColumn(_inputRaster1->name(), _inputRaster1->datadef().domain());
    newTable->addColumn(_inputRaster2->name(), _inputRaster2->datadef().domain());
    newTable->addColumn("NPix", IlwisObject::create<IDomain>("count"));
    newTable->addColumn("Area", IlwisObject::create<IDomain>("value"));
    _outputTable = newTable;

    return sPREPARED;
}

quint64 CrossRasters::createMetadata()
{
    QString url = QString("ilwis://operations/cross");
    Resource resource(QUrl(url), itOPERATIONMETADATA);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","crossing of two rastermaps");
    resource.addProperty("syntax","cross(raster1, raster2, ignoreundef1 | ignoreundef2 | ignoreundef | dontcare");
    resource.addProperty("description",TR("an overlay of two rasters. Pixels on the same positions are compared; the occurring combinations of values are stored"));
    resource.addProperty("inparameters","3");
    resource.addProperty("pin_1_type", itRASTER);
    resource.addProperty("pin_1_name", TR("first rastercoverage"));
    resource.addProperty("pin_1_desc",TR("input rastercoverage with domain item or numeric"));
    resource.addProperty("pin_2_type", itRASTER);
    resource.addProperty("pin_2_name", TR("second rastercoverage"));
    resource.addProperty("pin_2_desc",TR("input rastercoverage with domain item or numeric"));
    resource.addProperty("pin_3_type", itSTRING);
    resource.addProperty("pin_3_name", TR("undef handling"));
    resource.addProperty("pin_3_desc",TR("how undefs are handled can be defined per input raster"));
    resource.addProperty("outparameters","1|2");
    resource.addProperty("pout_1_type", itTABLE);
    resource.addProperty("pout_1_name", TR("output table"));
    resource.addProperty("pout_1_desc",TR("output table with the results of the cross operation"));
    resource.addProperty("pout_2_type", itRASTER);
    resource.addProperty("pout_2_name", TR("output raster"));
    resource.addProperty("pout_2_desc",TR("optional output raster with an item domain (the combinations)"));
    resource.prepare();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();
}
