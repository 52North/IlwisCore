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
#include "operationhelpergrid.h"
#include "geometryhelper.h"
#include "mirrorrotateraster.h"

using namespace Ilwis;
using namespace RasterOperations;

REGISTER_OPERATION(MirrorRotateRaster)

MirrorRotateRaster::MirrorRotateRaster()
{
}

MirrorRotateRaster::MirrorRotateRaster(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool MirrorRotateRaster::dimChanged(const PixelIterator& iter) const{
    switch(_method)    {
    case tmMirrorVertical:
    case tmRotate180:
    case tmRotate270:
    case tmMirrorDiagonal:
    case tmTranspose:
        return iter.ychanged();
    case tmRotate90:
    case tmMirrorHorizontal:
        return iter.xchanged();
    default:
        break;
    }
    return false;
}

void MirrorRotateRaster::translatepixels(PixelIterator iterIn, PixelIterator iterOut, quint32 linelength, int xstep, int ystep){
    std::vector<double> line(linelength);
    auto iterLine = line.begin();
    auto end = iterIn.end();
    for(; iterIn != end; ++iterIn, ++iterLine){
        if ( dimChanged(iterIn)){
            if (_method != tmTranspose)
                std::reverse(line.begin(), line.end());
            std::copy(line.begin(), line.end(),iterOut);
            iterLine = line.begin();
            iterOut = Pixel(iterOut.x() + xstep, iterOut.y() + ystep, iterOut.z());
        }
        (*iterLine) = *iterIn;
    }
    // lastline
    if (_method != tmTranspose)
        std::reverse(line.begin(), line.end());
    std::copy(line.begin(), line.end(),iterOut);
}

bool MirrorRotateRaster::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    PixelIterator itertt = PixelIterator(_inputRaster);


    std::function<bool(const BoundingBox)> Transform = [&](const BoundingBox box ) -> bool {
        if ( _method == tmMirrorVertical){
            translatepixels(PixelIterator(_inputRaster),PixelIterator(_outputRaster), _outputRaster->size().xsize(),  0,1);
        }
        else if ( _method == tmMirrorHorizontal){
            translatepixels(PixelIterator(_inputRaster,PixelIterator::fYXZ),PixelIterator(_outputRaster,PixelIterator::fYXZ), _outputRaster->size().ysize(),1,0);
        }
        else if ( _method == tmRotate90){
            translatepixels(PixelIterator(_inputRaster,PixelIterator::fYXZ),PixelIterator(_outputRaster), _outputRaster->size().xsize(),0,1);
        }
        else if ( _method == tmRotate180){
            PixelIterator iterOut(_outputRaster);
            iterOut.toEnd();
            iterOut -= box.xlength() - 1; // beginning of the lastline; iterator moves one line back each time
            translatepixels(PixelIterator(_inputRaster),iterOut, _outputRaster->size().xsize(),0,-1);
        }
        else if ( _method == tmRotate270){
            translatepixels(PixelIterator(_inputRaster),PixelIterator(_outputRaster,PixelIterator::fYXZ), _outputRaster->size().ysize(),1,0);
        }
        else if ( _method == tmMirrorDiagonal){
            PixelIterator iterOut(_outputRaster,PixelIterator::fYXZ);
            iterOut = Pixel(box.xlength() - 1,0,0);
            translatepixels(PixelIterator(_inputRaster),iterOut, _outputRaster->size().ysize(),-1,0);
        }
        else if ( _method == tmTranspose){
            translatepixels(PixelIterator(_inputRaster),PixelIterator(_outputRaster,PixelIterator::fYXZ), _outputRaster->size().ysize(),1,0);
       }
        return true;

    };

    bool ok = OperationHelperRaster::execute(ctx, Transform, _outputRaster);

    if ( ok && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(_outputRaster);
        logOperation(_outputRaster,_expression);
        ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER, _outputRaster->resource() );
    }
    return ok;
}

Ilwis::OperationImplementation *MirrorRotateRaster::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new MirrorRotateRaster(metaid, expr);
}

Ilwis::OperationImplementation::State MirrorRotateRaster::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString raster = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();
    QString method = _expression.parm(1).value().toLower();

    if (!_inputRaster.prepare(raster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
        return sPREPAREFAILED;
    }
    std::map<QString, TransPoseMethod> methods={{"mirrhor",tmMirrorHorizontal},{"mirrvert",tmMirrorVertical},
                                                {"mirrdiag",tmMirrorDiagonal},{"transpose",tmTranspose},{"rotate90",tmRotate90},
                                                {"rotate180",tmRotate180},{"rotate270",tmRotate270}};
    auto iter = methods.find(method);
    if ( iter == methods.end()){
        ERROR2(ERR_NOT_FOUND2,method, TR("in method for mirrorrotate"));
        return sPREPAREFAILED;
    }
    _method = iter->second;
    Size<> sz = _inputRaster->size();
    Envelope outputenv = _inputRaster->envelope();

    if ( _method == tmTranspose || _method == tmRotate90 || _method == tmRotate270 || _method == tmMirrorDiagonal){
        sz = Size<>(sz.ysize(), sz.xsize(), sz.zsize());
        Coordinate center = (outputenv.max_corner() + outputenv.min_corner()) / 2.0;
        auto rotated = GeometryHelper::rotate2d(center,90,(std::vector<Coordinate>)outputenv);
        outputenv = {rotated};
    }

    _outputRaster = OperationHelperRaster::initialize(_inputRaster,itRASTER,itCOORDSYSTEM | itDOMAIN);
    _outputRaster->gridRef()->prepare(0,sz);

    QString grfs = QString("code=georef:type=corners,csy=%1,envelope=%2,gridsize=%3")
            .arg(_outputRaster->coordinateSystem()->id())
            .arg(outputenv.toString())
            .arg(sz.toString());
    _outputRaster->georeference(grfs);
    if (outputName != sUNDEF)
        _outputRaster->name(outputName);


    return sPREPARED;
}

quint64 MirrorRotateRaster::createMetadata()
{
    OperationResource operation({"ilwis://operations/mirrorrotateraster"});
    operation.setSyntax("mirrorrotateraster(inputraster,mirrortype=mirrhor | mirrvert | mirrdiag | transpose | rotate90 | rotate180 | rotate270)");
    operation.setDescription(TR("transpose the raster according to the method indicated by the second parameter"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itRASTER,  TR("input raster"),TR("ratser to be transposed"));
    operation.addInParameter(1,itSTRING, TR("transpose method"),TR("rotation or mirror of the input map"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output raster"), TR("output raster with a new georef"));
    operation.setKeywords("raster, geometry");

    mastercatalog()->addItems({operation});
    return operation.id();
}
