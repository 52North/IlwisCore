#include <QString>
#include <QVector>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include "kernel.h"
#include "ilwis.h"
#include "angle.h"
#include "point.h"
#include "box.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "domain.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "coordinatesystem.h"
#include "valuedefiner.h"
#include "columndefinition.h"
#include "table.h"
#include "containerstatistics.h"
#include "coverage.h"
#include "georeference.h"
#include "connectorinterface.h"
#include "grid.h"
#include "gridcoverage.h"
#include "pixeliterator.h"
#include "resource.h"
#include "blockiterator.h"

using namespace Ilwis;

GridBlock::GridBlock(BlockIterator& iter) :
    _iterator(iter)
{
    // for efficiency the blocks and offsets are precalculated at the cost of some memory
    // when calculating the linear postions only very basic operations are needed then
    int ysize = iter._raster->size().ysize();
    int blockYSize = iter._raster->_grid->maxLines();
    int blockXSize = iter._raster->_grid->size().xsize();
    _internalBlockNumber.resize(ysize);
    _offsets.resize(ysize);
    qint32 base = 0;
    for(int i=0; i < ysize; ++i ) {
        if ( i == blockYSize)
            base = 0;
        _internalBlockNumber[i] = i / blockYSize;
        _offsets[i] = base;
        base += blockXSize;
    }
}

double& GridBlock::operator ()(quint32 x, quint32 y, quint32 z)
{
    if ( _iterator._outside != rILLEGAL) {
        _iterator._outside = rILLEGAL;
    }

    const Size& sz = _iterator._blocksize;
    if ( sz.contains(x,y,z)) {
        qint32 ypos = _iterator._y + y;
        return _iterator._raster->_grid->value(_internalBlockNumber[ypos ], _offsets[ypos] + x);
    }
    ERROR2(ERR_ILLEGAL_VALUE_2, "block position", QString("%1,%2,%3").arg(x,y,z));
    return _iterator._outside;
}


BlockIterator::BlockIterator(IGridCoverage raster, const Size &sz, const Box3D<> &box, double step) :
    PixelIterator(raster,box,step),
    _block(*this),
    _blocksize(sz)

{

}

BlockIterator& BlockIterator::operator ++()
{
    moveXYZ(1);
    return *this;
}

BlockIterator &BlockIterator::operator --()
{
    moveXYZ(-1);
    return *this;
}
