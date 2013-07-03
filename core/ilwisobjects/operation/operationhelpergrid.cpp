#include "kernel.h"
#include "raster.h"
#include "connectorinterface.h"
#include "symboltable.h"
#include "ilwisoperation.h"

using namespace Ilwis;

OperationHelperRaster::OperationHelperRaster()
{
}

Box3D<qint32> OperationHelperRaster::initialize(const IGridCoverage &inputGC, IGridCoverage &outputGC, const Parameter& parm, quint64 what)
{
    Resource resource(itGRID);
    Size sz = inputGC->size();
    Box3D<qint32> box(sz);

    if ( what & itGRIDSIZE) {
        resource.addProperty("size", IVARIANT(sz));
    }
    if ( what & itENVELOPE) {
        if ( box.isNull() || !box.isValid()) {
            sz = inputGC->size();
            box  = Box3D<qint32>(sz);
        }
        Box2D<double> bounds = inputGC->georeference()->pixel2Coord(box);
        resource.addProperty("envelope", IVARIANT(bounds));
    }
    if ( what & itCOORDSYSTEM) {
        resource.addProperty("coordinatesystem", IVARIANT(inputGC->coordinateSystem()));
    }

    if ( what & itGEOREF) {
        if ( box.isNull() || !box.isValid()) {
            sz = inputGC->size();
            box  = Box3D<qint32>(sz);
        }
        if ( sz.xsize() == box.xlength() && sz.ysize() == box.ylength())
            resource.addProperty("georeference", IVARIANT(inputGC->georeference()));
    }
    if ( what & itDOMAIN) {
        resource.addProperty("domain", IVARIANT(inputGC->datadef().domain()));
    }
    resource.prepare();

    outputGC.prepare(resource);
    if ( what & itTABLE) {
        if ( inputGC->attributeTable(itGRID).isValid())    {
            if ( inputGC->datadef().domain() == outputGC->datadef().domain()) {
                if ( outputGC.isValid())
                    outputGC->attributeTable(itGRID,inputGC->attributeTable(itGRID));
            }
        }
    }

    return box;
}

IIlwisObject OperationHelperRaster::initialize(const IIlwisObject &inputObject, IlwisTypes tp, quint64 what)
{
    Resource resource(tp);
    if (inputObject->ilwisType() & itCOVERAGE) {
        if (inputObject->ilwisType() == itGRID) {
            IGridCoverage gcInput = inputObject.get<GridCoverage>();
            if ( what & itGRIDSIZE) {
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

int OperationHelperRaster::subdivideTasks(ExecutionContext *ctx,const IGridCoverage& gcov, const Box3D<qint32> &bnds, std::vector<Box3D<qint32> > &boxes)
{
    if ( !gcov.isValid() || gcov->size().isNull() || gcov->size().ysize() == 0) {
        return ERROR1(ERR_NO_INITIALIZED_1, "Grid size");
        return iUNDEF;
    }

    int cores = std::min(QThread::idealThreadCount(),gcov->size().ysize());
    if (gcov->size().totalSize() < 10000 || ctx->_threaded == false)
        cores = 1;

    boxes.clear();
    boxes.resize(cores);
    Box3D<qint32> bounds = bnds;
    if ( bounds.isNull())
        bounds = Box3D<qint32>(gcov->size());
    int left = 0; //bounds.min_corner().x();
    int right = bounds.size().xsize();
    int top = bounds.size().ysize();
    int step = bounds.size().ysize() / cores;
    int currentY = 0;

    for(int i=0 ; i < cores; ++i){
        Box3D<qint32> smallBox(Pixel(left, currentY), Pixel(right - 1, std::min(top - 1,currentY + step)) );
        boxes[i] = smallBox;
        currentY = currentY + step  ;
    }
    return cores;
}

