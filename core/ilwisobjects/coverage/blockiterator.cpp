#include "kernel.h"
#include "raster.h"
#include "pixeliterator.h"
#include "blockiterator.h"

using namespace Ilwis;

CellIterator::CellIterator(GridBlock *bl, quint32 start) : _block(bl), _position(start){

}

CellIterator &CellIterator::operator=(CellIterator &iter)
{
    _block = iter._block;
    _position = iter._position;

    return *this;
}

CellIterator CellIterator::operator++(int)
{
    CellIterator iter = *this;
    move(1);
    return iter;
}

CellIterator CellIterator::operator--(int)
{
    CellIterator iter = *this;
    move(-1);
    return iter;
}

void CellIterator::move(int n){
    const Size& sz = _block->size();
    _position += n;
    if ( _position >= sz.totalSize())
        _position = sz.totalSize();
    else if( _position < 0)
        _position = 0;
}

double &CellIterator::operator*()
{
    const Size& sz = _block->size();
    quint32 size2d = sz.xsize() * sz.ysize();
    int z = _position / size2d;
    int y = (_position - z * size2d) / sz.xsize();
    int x = _position - z * size2d - y * sz.xsize();

    return (*_block)(x,y,z);
}

qint32 CellIterator::position() const
{
    return _position;
}

Size CellIterator::blocksize() const
{
    return _block->size();
}

bool operator==(const CellIterator& iter1, const CellIterator& iter2) {
    return iter1.blocksize() == iter2.blocksize() && iter1.position() == iter2.position();
}

bool operator!=(const CellIterator& iter1, const CellIterator& iter2) {
    return ! operator==(iter1, iter2);
}

//--------------------------------------------------------
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

Size GridBlock::size() const
{
    return _iterator.blockSize();
}

CellIterator GridBlock::begin()
{
    return CellIterator(this);
}

CellIterator GridBlock::end()
{
    return CellIterator(this, size().totalSize());
}


BlockIterator::BlockIterator(IGridCoverage raster, const Size &sz, const Box3D<> &box, double step) :
    PixelIterator(raster,box,step),
    _block(*this),
    _blocksize(sz)

{

}

BlockIterator& BlockIterator::operator ++()
{
    moveXYZ(_blocksize.xsize());
    return *this;
}

BlockIterator &BlockIterator::operator --()
{
    moveXYZ(-_blocksize.xsize());
    return *this;
}

Size BlockIterator::blockSize() const
{
    return _blocksize;
}
