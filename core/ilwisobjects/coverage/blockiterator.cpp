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

    double &v =  (*_block)(x,y,z);
    return v;
}

qint32 CellIterator::position() const
{
    return _position;
}

Size CellIterator::blocksize() const
{
    return _block->size();
}

//--------------------------------------------------------
GridBlock::GridBlock(BlockIterator& iter) :
    _iterator(iter)
{
    // for efficiency the blocks and offsets are precalculated at the cost of some memory
    // when calculating the linear postions only very basic operations are needed then
    int ysize = iter._raster->size().ysize();
    _blockYSize = iter._raster->_grid->maxLines();
    _blockXSize = iter._raster->_grid->size().xsize();
    _internalBlockNumber.resize(ysize);
    _offsets.resize(ysize);
    qint32 base = 0;
    for(int i=0; i < ysize; ++i ) {
        if ( i == _blockYSize)
            base = 0;
        _internalBlockNumber[i] = i / _blockYSize;
        _offsets[i] = base;
        base += _blockXSize;
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
        quint32 block = ypos/ _blockYSize;
        ypos = ypos % _blockYSize;
        return _iterator._raster->_grid->value(_internalBlockNumber[block ], _offsets[ypos] +  _iterator._x +  x);
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

BlockIterator::BlockIterator(quint64 endpos) : PixelIterator(endpos), _block(*this)
{

}

BlockIterator& BlockIterator::operator ++()
{
    quint32 dist = _blocksize.xsize();
    if ( _y + dist  > _endy) {
        dist = 1e9; // big number, force and end to the iteration
    } else {
        if ( _x + dist * 2 > _endx) {
            dist = 1 + _endx - _x + ( (_endx - _box.min_corner().x() + 1) * ( _block.size().ysize() - 1));

        }
    }
    move(dist);
    return *this;

}

BlockIterator &BlockIterator::operator --()
{
    move(-_blocksize.xsize());
    return *this;
}

BlockIterator BlockIterator::end() const
{
    return BlockIterator(_endpositionid);
}

bool BlockIterator::operator==(const BlockIterator &iter) const
{
    return PixelIterator::operator ==(iter);
}

bool BlockIterator::operator!=(const BlockIterator& iter) const{
    return ! operator ==(iter);
}

Size BlockIterator::blockSize() const
{
    return _blocksize;
}



