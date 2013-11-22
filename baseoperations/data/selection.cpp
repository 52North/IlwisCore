#include "kernel.h"
#include "raster.h"
#include "georefimplementation.h"
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
    IRasterCoverage outputRaster = _outputObj.get<RasterCoverage>();
    IRasterCoverage inputRaster = _inputObj.get<RasterCoverage>();

    BoxedAsyncFunc selection = [&](const Box3D<qint32>& box ) -> bool {
        Box3D<qint32> inpbox = box.size();
        inpbox += _base;
        inpbox += std::vector<qint32>{0, box.min_corner().y(),0};
        if ( _zvalue == iUNDEF)
            inpbox.copyFrom(box, Box3D<>::dimZ);
        PixelIterator iterOut(outputRaster, box);
        PixelIterator iterIn(inputRaster, inpbox);

        AttributeRecord rec;
        if ( _attribColumn != "")
            rec = AttributeRecord(inputRaster->attributeTable(), COVERAGEKEYCOLUMN);

        double v_in = 0;
        for_each(iterOut, iterOut.end(), [&](double& v){
            v_in = *iterIn;
            if ( v_in != rUNDEF) {
                if ( rec.isValid()) {
                    quint32 colIndex  = rec.columnIndex(_attribColumn);
                    QVariant var = rec.cellByKey(v_in,colIndex);
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

    ctx->_threaded = false;
    bool resource = OperationHelperRaster::execute(ctx,selection, outputRaster, _box);

    if ( resource && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(outputRaster);
        ctx->addOutput(symTable, value, outputRaster->name(), itRASTER,outputRaster->source());
    }
    return resource;


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
    IlwisTypes inputType = itRASTER;
    QString raster = _expression.parm(0).value();
    if (!_inputObj.prepare(raster, inputType)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
        return sPREPAREFAILED;
    }
    IRasterCoverage inputRaster = _inputObj.get<RasterCoverage>();
    quint64 copylist = itCOORDSYSTEM;


    QString selector = _expression.parm(1).value();
    selector = selector.remove('"');

    int index = selector.indexOf("box=");
    Box2D<double> box;
    if ( index != -1) {
        QString crdlist = "box(" + selector.mid(index+4) + ")";
        _box = Box3D<qint32>(crdlist);
        box = inputRaster->georeference()->pixel2Coord(_box);
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
        if (! inputRaster->attributeTable().isValid()) {
            ERROR2(ERR_NO_FOUND2,"attribute-table", "coverage");
            return sPREPAREFAILED;
        }
        _attribColumn =  selector.mid(index+10);
        copylist |= itRASTERSIZE | itGEOREF | itENVELOPE;
    }
    int indexindex = selector.indexOf("index=");
    if ( indexindex != -1) {
        copylist |= itDOMAIN | itGEOREF | itENVELOPE | itTABLE;
        _box = Box3D<>(inputRaster->size());
        QString zvalues = selector.mid(6);
        bool ok;
        _zvalue = zvalues.toInt(&ok);
        if ( !ok || _zvalue < 0) {
            ERROR3(ERR_ILLEGAL_PARM_3, TR("layer index"), zvalues,"Selection");
            return sPREPAREFAILED;
        }
        _box.min_corner().z(_zvalue);
        _box.max_corner().z(_zvalue);
        std::vector<qint32> vec{_box.min_corner().x(), _box.min_corner().y(),_box.min_corner().z()};
        _base = vec;
    }

     _outputObj = OperationHelperRaster::initialize(_inputObj,inputType, copylist);
     if ( !_outputObj.isValid()) {
         ERROR1(ERR_NO_INITIALIZED_1, "output coverage");
         return sPREPAREFAILED;
     }
     IRasterCoverage outputRaster = _outputObj.get<RasterCoverage>();
     if ( (copylist & itDOMAIN) == 0) {
         outputRaster->datadef() = _attribColumn != "" ? inputRaster->attributeTable()->columndefinition(_attribColumn).datadef()
                                                   : outputRaster->datadef() = inputRaster->datadef();
     }
     QString outputName = _expression.parm(0,false).value();
     if ( outputName != sUNDEF)
         _outputObj->setName(outputName);
     if ( (copylist & itGEOREF) == 0) {
        Resource resource(QUrl("ilwis://internalcatalog/georeference"),itGEOREF);
        resource.addProperty("size", IVARIANT(_box.size()));
        resource.addProperty("envelope", IVARIANT(box));
        resource.addProperty("coordinatesystem", IVARIANT(inputRaster->coordinateSystem()));
        resource.addProperty("name", _outputObj->name());
        resource.addProperty("centerofpixel",inputRaster->georeference()->centerOfPixel());
        IGeoReference  grf;
        grf.prepare(resource);
        outputRaster->georeference(grf);
        outputRaster->envelope(box);
    }
     if(indexindex != -1)  {
         Size sz(outputRaster->size().xsize(),outputRaster->size().xsize(), 1);
         outputRaster->size(sz);
     }

    return sPREPARED;
}

quint64 Selection::createMetadata()
{
    QString url = QString("ilwis://operations/selection");
    Resource resource(QUrl(url), itOPERATIONMETADATA);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","selection");
    resource.addProperty("syntax","selection(coverage,selection-definition)");
    resource.addProperty("description",TR("the operation select parts of the spatial extent or attributes to create a 'smaller' coverage"));
    resource.addProperty("inparameters","2");
    resource.addProperty("pin_1_type", itRASTER);
    resource.addProperty("pin_1_name", TR("input rastercoverage"));
    resource.addProperty("pin_1_desc",TR("input rastercoverage with a domain as specified by the selection"));
    resource.addProperty("pin_2_type", itSTRING);
    resource.addProperty("pin_2_name", TR("selection-definition"));
    resource.addProperty("pin_2_desc",TR("Selection can either be attribute, layer index or area definition (e.g. box)"));
    resource.addProperty("pout_1_type", itRASTER);
    resource.addProperty("pout_1_name", TR("rastercoverage were the selection has been applied"));
    resource.addProperty("pout_1_desc",TR(""));
    resource.prepare();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();

}

