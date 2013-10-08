#include "kernel.h"
#include "raster.h"
#include "pixeliterator.h"
#include "blockiterator.h"

using namespace Ilwis;


CellIterator::CellIterator(GridBlock *bl, bool end) :
    _block(bl),
    _positionx(end ? _block->size().xsize() : 0),
    _positiony(end ? _block->size().ysize() : 0),
    _positionz(end ? _block->size().zsize() : 0)
{

}

CellIterator &CellIterator::operator=(CellIterator &iter)
{
    _block = iter._block;
    _positionx = iter._positionx;
    _positiony = iter._positiony;
    _positionz = iter._positionz;

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
    if ( _positionx < sz.xsize() - 1)
        ++_positionx;
    else if ( _positiony < sz.ysize() - 1) {
        ++_positiony;
        _positionx = 0;
    } else if ( _positionz < sz.zsize() - 1) {
        ++_positionz;
        _positionx = _positiony = 0;
    } else {
        _positionx = sz.xsize();
        _positiony = sz.ysize();
        _positionz = sz.zsize();
    }
}

double &CellIterator::operator*()
{
    double &v =  (*_block)(_positionx,_positiony,_positionz);
    return v;
}


Size CellIterator::blocksize() const
{
    return _block->size();
}

bool Ilwis::operator==(const CellIterator& iter1, const CellIterator& iter2) {
    return iter1.blocksize() == iter2.blocksize() &&
            iter1._positionx== iter2._positionx &&
            iter1._positiony== iter2._positiony &&
            iter1._positionz== iter2._positionz;
}

bool Ilwis::operator!=(const CellIterator& iter1, const CellIterator& iter2) {
    return ! Ilwis::operator==(iter1, iter2);
}


//--------------------------------------------------------
GridBlock::GridBlock(BlockIterator& iter) :
    _iterator(iter)
{
    // for efficiency the blocks and offsets are precalculated at the cost of some memory
    // when calculating the linear postions only very basic operations are needed then
    if (! isValid())
        return ;
    int ysize = iter._raster->size().ysize();
    _blockYSize = iter._raster->_grid->maxLines();
    _blockXSize = iter._raster->_grid->size().xsize();
    _XYSize = iter._raster->_grid->size().xsize() * iter._raster->_grid->size().ysize();
    _internalBlockNumber.resize(ysize);
    _offsets.resize(ysize);
    qint32 base = 0;
    for(int i=0; i < ysize; ++i ) {
        if ( i % _blockYSize == 0)
            base = 0;
        _internalBlockNumber[i] =   i / _blockYSize;
        _offsets[i] = base;
        base += _blockXSize;
    }
    _bandOffset = iter._raster->_grid->blocksPerBand();
}

double& GridBlock::operator ()(quint32 x, quint32 y, quint32 z)
{
    if ( !isValid())
        throw ErrorObject(TR("Using invalid pixeliterator, are all data sources accessible?"));
    if ( _iterator._outside != rILLEGAL) {
        _iterator._outside = rILLEGAL;
    }

    quint32 zbase = _iterator._z + z;
    qint32 ypos = _iterator._y + y;
    double &v = _iterator._raster->_grid->value(_internalBlockNumber[ypos ] + _bandOffset * zbase, _offsets[ypos] +  _iterator._x +  x);
    return v;

}

Size GridBlock::size() const
{
    return _iterator.blockSize();
}

CellIterator GridBlock::begin()
{
    return CellIterator(this, false);
}

CellIterator GridBlock::end()
{
    return CellIterator(this, true);
}

const BlockIterator &GridBlock::iterator() const
{
    return _iterator;
}

bool GridBlock::isValid() const
{
    return _iterator.isValid();
}

GridBlock::operator std::vector<double>()
{
    const Size& sz = _iterator.blockSize();
    int count = 0;
    std::vector<double> v(sz.totalSize());
    for(quint32 z=0; z < sz.zsize(); ++z) {
        for(quint32 y=0; z < sz.ysize(); ++y) {
            for(quint32 x=0; z < sz.xsize(); ++x) {
                v[count++] = operator ()(x,y,z);
            }
        }
    }
    return v;
}

//----------------------------------------------------------------------------------------------
BlockIterator::BlockIterator(IRasterCoverage raster, const Size &sz, const Box3D<> &box) :
    PixelIterator(raster,box),
    _block(*this),
    _blocksize(sz),
    _stepsizes(sz)

{
}

BlockIterator::BlockIterator(quint64 endpos) : PixelIterator(endpos), _block(*this)
{

}

BlockIterator& BlockIterator::operator ++()
{
    quint32 dist = _blocksize.xsize();
    if ( _y + dist - 1 > _endy) {
        dist = linearPosition() + dist + 1;
    } else {
        if ( _x + dist >= _endx) {
            dist = 1 + _endx - _x + ( _box.xlength()) * ( _block.size().ysize() - 1);
        }

    }
    move(dist);
    if ( zchanged()) {
        dist = _box.xlength() * _box.ylength() * (_blocksize.zsize() - 1);
        move(dist);
    }

    return *this;

}

BlockIterator &BlockIterator::operator --()
{
    move(-_blocksize.xsize());
    return *this;
}

BlockIterator BlockIterator::end() const
{
    return BlockIterator(_endposition);
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

void BlockIterator::stepsizes(const Size &stepsizes)
{
    _stepsizes = stepsizes;
}





