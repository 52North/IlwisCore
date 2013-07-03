#include "kernel.h"
#include "raster.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "pixeliterator.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "selection.h"

using namespace Ilwis;
using namespace BaseOperations;

Selection::Selection()
{
}


Selection::Selection(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

Selection::~Selection()
{
}


bool Selection::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    IGridCoverage outputGC = _outputObj.get<GridCoverage>();
    IGridCoverage inputGC = _inputObj.get<GridCoverage>();

    BoxedAsyncFunc selection = [&](const Box3D<qint32>& box ) -> bool {
        Box3D<qint32> inpbox = box.size();
        inpbox += _base;
        inpbox += std::vector<qint32>{0, box.min_corner().y(),0};
        inpbox.copyFrom(box, Box3D<>::dimZ);
        PixelIterator iterOut(outputGC, box);
        PixelIterator iterIn(inputGC, inpbox);

        AttributeRecord rec;
        if ( _attribColumn != "")
            rec = AttributeRecord(inputGC->attributeTable(itCOVERAGE), "coverage_key");

        double v_in = 0;
        for_each(iterOut, iterOut.end(), [&](double& v){
            v_in = *iterIn;
            if ( v_in != rUNDEF) {
                if ( rec.isValid()) {
                    QVariant var = rec.cellByKey(v_in,_attribColumn);
                    v = var.toDouble();
                    if ( isNumericalUndef(v))
                        v = rUNDEF;
                } else {
                    v = v_in;
                }
            }
            ++iterIn;
            ++iterOut;
        }
        );
        return true;
    };

    bool res = OperationHelperRaster::execute(ctx,selection, outputGC, _box);

    if ( res && ctx != 0) {
        QVariant value;
        value.setValue<IGridCoverage>(outputGC);
        ctx->addOutput(symTable, value, outputGC->name(), itGRID,outputGC->source());
    }
    return res;


}

Ilwis::OperationImplementation *Selection::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Selection(metaid, expr);
}


Ilwis::OperationImplementation::State Selection::prepare(ExecutionContext *, const SymbolTable &)
{
    if ( _expression.parameterCount() != 2) {
        ERROR3(ERR_ILLEGAL_NUM_PARM3,"rasvalue","1",QString::number(_expression.parameterCount()));
        return sPREPAREFAILED;
    }
    IlwisTypes inputType = itGRID;
    QString gc = _expression.parm(0).value();
    if (!_inputObj.prepare(gc, inputType)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,gc,"");
        return sPREPAREFAILED;
    }
    IGridCoverage inputGC = _inputObj.get<GridCoverage>();
    quint64 copylist = itCOORDSYSTEM;


    QString selector = _expression.parm(1).value();
    selector = selector.remove('"');

    int index = selector.indexOf("box=");
    Box2D<double> box;
    if ( index != -1) {
        QString crdlist = "box(" + selector.mid(index+4) + ")";
        _box = Box3D<qint32>(crdlist);
        box = inputGC->georeference()->pixel2Coord(_box);
        copylist |= itDOMAIN | itTABLE;
        std::vector<qint32> vec{_box.min_corner().x(), _box.min_corner().y(),_box.min_corner().z()};
        _base = vec;

    }
    index = selector.indexOf("polygon=");
    if ( index != -1)
    {
        //TODO
        copylist |= itDOMAIN | itTABLE;
    }
    index = selector.indexOf("attribute=");
    if ( index != -1 ) {
        if (! inputGC->attributeTable(itCOVERAGE).isValid()) {
            ERROR2(ERR_NO_FOUND2,"attribute-table", "coverage");
            return sPREPAREFAILED;
        }
        _attribColumn =  selector.mid(index+10);
        copylist |= itGRIDSIZE | itGEOREF | itENVELOPE;
    }

     _outputObj = OperationHelperRaster::initialize(_inputObj,inputType, copylist);
     if ( !_outputObj.isValid()) {
         ERROR1(ERR_NO_INITIALIZED_1, "output coverage");
         return sPREPAREFAILED;
     }
     IGridCoverage outputGC = _outputObj.get<GridCoverage>();
     if ( (copylist & itDOMAIN) == 0) {
         outputGC->datadef() = _attribColumn != "" ? inputGC->attributeTable(itCOVERAGE)->columndefinition(_attribColumn).datadef()
                                                   : outputGC->datadef() = inputGC->datadef();
     }
     QString outputName = _expression.parm(0,false).value();
     if ( outputName != sUNDEF)
         _outputObj->setName(outputName);
     if ( (copylist & itGEOREF) == 0) {
        Resource res(QUrl("ilwis://internal/georeference"),itCORNERSGEOREF);
        res.addProperty("size", IVARIANT(_box.size()));
        res.addProperty("envelope", IVARIANT(box));
        res.addProperty("coordinatesystem", IVARIANT(inputGC->coordinateSystem()));
        res.addProperty("name", _outputObj->name());
        res.addProperty("centerofpixel",inputGC->georeference()->centerOfPixel());
        IGeoReference  grf;
        grf.prepare(res);
        outputGC->georeference(grf);
        outputGC->envelope(box);
    }

    return sPREPARED;
}

quint64 Selection::createMetadata()
{
    QString url = QString("ilwis://operations/selection");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","selection");
    res.addProperty("syntax","selection(coverage,selection-definition)");
    res.addProperty("inparameters","2");
    res.addProperty("pin_1_type", itGRID);
    res.addProperty("pin_1_name", TR("input gridcoverage"));
    res.addProperty("pin_1_desc",TR("input gridcoverage with a domain as specified by the selection"));
    res.addProperty("pin_2_type", itSTRING);
    res.addProperty("pin_2_name", TR("selection-definition"));
    res.addProperty("pin_2_desc",TR("Selection can either be attribute, layer index or area definition (e.g. box)"));
    res.addProperty("pout_1_type", itGRID);
    res.addProperty("pout_1_name", TR("gridcoverage were the selection has been applied"));
    res.addProperty("pout_1_desc",TR(""));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();

}

