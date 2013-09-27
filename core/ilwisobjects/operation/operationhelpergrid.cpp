#include "kernel.h"
#include "raster.h"
#include "connectorinterface.h"
#include "symboltable.h"
#include "ilwisoperation.h"

using namespace Ilwis;

OperationHelperRaster::OperationHelperRaster()
{
}

Box3D<qint32> OperationHelperRaster::initialize(const IRasterCoverage &inputRaster, IRasterCoverage &outputRaster, const Parameter& parm, quint64 what)
{
    Resource resource(itRASTER);
    Size sz = inputRaster->size();
    Box3D<qint32> box(sz);

    if ( what & itRASTERSIZE) {
        resource.addProperty("size", IVARIANT(sz));
    }
    if ( what & itENVELOPE) {
        if ( box.isNull() || !box.isValid()) {
            sz = inputRaster->size();
            box  = Box3D<qint32>(sz);
        }
        Box2D<double> bounds = inputRaster->georeference()->pixel2Coord(box);
        resource.addProperty("envelope", IVARIANT(bounds));
    }
    if ( what & itCOORDSYSTEM) {
        resource.addProperty("coordinatesystem", IVARIANT(inputRaster->coordinateSystem()));
    }

    if ( what & itGEOREF) {
        if ( box.isNull() || !box.isValid()) {
            sz = inputRaster->size();
            box  = Box3D<qint32>(sz);
        }
        if ( sz.xsize() == box.xlength() && sz.ysize() == box.ylength())
            resource.addProperty("georeference", IVARIANT(inputRaster->georeference()));
    }
    if ( what & itDOMAIN) {
        resource.addProperty("domain", IVARIANT(inputRaster->datadef().domain()));
    }
    resource.prepare();

    outputRaster.prepare(resource);
    if ( what & itTABLE) {
        if ( inputRaster->attributeTable().isValid())    {
            if ( inputRaster->datadef().domain() == outputRaster->datadef().domain()) {
                if ( outputRaster.isValid())
                    outputRaster->attributeTable(inputRaster->attributeTable());
            }
        }
    }

    return box;
}

IIlwisObject OperationHelperRaster::initialize(const IIlwisObject &inputObject, IlwisTypes tp, quint64 what)
{
    Resource resource(tp);
    if (inputObject->ilwisType() & itCOVERAGE) {
        if (inputObject->ilwisType() == itRASTER) {
            IRasterCoverage gcInput = inputObject.get<RasterCoverage>();
            if ( what & itRASTERSIZE) {
                Size sz = gcInput->size();
                Box3D<qint32> box(sz);
                resource.addProperty("size", IVARIANT(box.size()));
            }
            if ( what & itGEOREF) {
                resource.addProperty("georeference", IVARIANT(gcInput->georeference()));
            }
        }
     }

    resource.prepare();
    IIlwisObject obj;
    obj.prepare(resource);
    if (inputObject->ilwisType() & itCOVERAGE) {
        OperationHelper::initialize(inputObject, obj, tp, what);
    }

    return obj;
}

int OperationHelperRaster::subdivideTasks(ExecutionContext *ctx,const IRasterCoverage& raster, const Box3D<qint32> &bnds, std::vector<Box3D<qint32> > &boxes)
{
    if ( !raster.isValid() || raster->size().isNull() || raster->size().ysize() == 0) {
        return ERROR1(ERR_NO_INITIALIZED_1, "Grid size");
        return iUNDEF;
    }

    int cores = std::min(QThread::idealThreadCount(),raster->size().ysize());
    if (raster->size().totalSize() < 10000 || ctx->_threaded == false)
        cores = 1;

    boxes.clear();
    boxes.resize(cores);
    Box3D<qint32> bounds = bnds;
    if ( bounds.isNull())
        bounds = Box3D<qint32>(raster->size());
    int left = 0; //bounds.min_corner().x();
    int right = bounds.size().xsize();
    int top = bounds.size().ysize();
    int step = bounds.size().ysize() / cores;
    int currentY = 0;

    for(int i=0 ; i < cores; ++i){
        Box3D<qint32> smallBox(Voxel(left, currentY,0), Voxel(right - 1, std::min(top - 1,currentY + step),bounds.zlength()) );
        boxes[i] = smallBox;
        currentY = currentY + step  ;
    }
    return cores;
}

