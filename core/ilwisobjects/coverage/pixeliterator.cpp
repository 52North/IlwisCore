#include "kernel.h"
#include "raster.h"
#include "linerasterizer.h"
#include "tranquilizer.h"
#include "pixeliterator.h"


using namespace Ilwis;

PixelIterator::PixelIterator() :
    _grid(0),
    _x(0),
    _y(0),
    _z(0),
    _localOffset(0),
    _currentBlock(0),
    _flow(fXYZ),
    _isValid(true),
    _endx(0),
    _endy(0),
    _endz(0),
    _linearposition(0),
    _endposition(0),
    _xChanged(false),
    _yChanged(false),
    _zChanged(false)
{

}

PixelIterator::PixelIterator(const IRasterCoverage &raster, const Box3D<>& box) :
    _raster(raster),
    _box(box),
    _localOffset(0),
    _currentBlock(0),
    _flow(fXYZ),
    _isValid(false)
{
    init();
}


PixelIterator::PixelIterator(PixelIterator&& iter) :
    _raster(std::move(iter._raster)),
    _grid(std::move(iter._grid)),
    _box(std::move(iter._box)),
    _x(iter._x),
    _y(iter._y),
    _z(iter._z),
    _localOffset(iter._localOffset),
    _currentBlock(iter._currentBlock),
    _flow(iter._flow),
    _isValid(iter._isValid),
    _endx(iter._endx),
    _endy(iter._endy),
    _endz(iter._endz),
    _linearposition(iter._linearposition),
    _endposition(iter._endposition),
    _xChanged(iter._xChanged),
    _yChanged(iter._yChanged),
    _zChanged(iter._zChanged)
{
//    _raster = IRasterCoverage();
//    _box = Box3D<>();
//    _localOffset = 0;
//    _currentBlock = 0;
//    _flow = fXYZ;
//    _isValid = false;
}

PixelIterator::PixelIterator(const PixelIterator& iter)  {
    copy(iter);
}


void PixelIterator::copy(const PixelIterator &iter) {
    _raster = iter._raster;
    if ( _raster.isValid()) // TODO beyond end marker(end()) dont have a valid raster, no problem just yet but it maybe needed in the future
        _grid = _raster->_grid.data();
    _box = iter._box;
    _isValid = iter._isValid;
    _flow = iter._flow;
    _x = iter._x;
    _y = iter._y;
    _z = iter._z;
    _endx = iter._endx;
    _endy = iter._endy;
    _endz = iter._endz;
    _linearposition = iter._linearposition;
    _endposition = iter._endposition;
    _localOffset = iter._localOffset;
    _currentBlock = iter._currentBlock;
    _trq = iter._trq;

}

void PixelIterator::init() {
    const Size& sz = _raster->size();
    if ( _box.isNull()) {
        _box = Box3D<>(sz);
    }
    _box.ensure(sz);

    _x = _box.min_corner().x();
    _y = _box.min_corner().y();
    _z = _box.min_corner().z();

    _endx = _box.max_corner().x();
    _endy = _box.max_corner().y();
    _endz = _box.max_corner().z();

    bool inside = contains(Pixel(_x,_y));
    if ( inside) {
        _grid = _raster->grid();
    }
    if ( _grid == 0) {
        _isValid = false;
        if (!isValid())
            throw ErrorObject(TR("Using invalid pixeliterator, are all data sources accessible?"));
    }

    initPosition();
    _isValid = inside;
    _xChanged = _yChanged = _zChanged = false;
}

inline bool PixelIterator::moveXYZ(int delta) {
    _x += delta;
    _linearposition += delta;
    _localOffset += delta;
    _xChanged = true;
    _yChanged = _zChanged = false;

    if ( _x > _endx) {
        _xChanged = (_x - delta) %  _box.xlength() != 0;
        qint32 tempy = _y + (_x - _box.min_corner().x()) / _box.xlength();
        _x = _box.min_corner().x() + (_x - _box.min_corner().x()) % _box.xlength();
        _yChanged = tempy != _y;
        std::swap(_y,tempy);
        qint32 ylocal = _y % _grid->maxLines();
        quint32 localblock = _y /  _grid->maxLines();
        quint32 bandblocks = _grid->blocksPerBand() * _z;
        _localOffset = _x + ylocal * _grid->size().xsize();
        if (_trq)
            _trq->move();
        if ( bandblocks + localblock != _currentBlock) {
            _currentBlock = bandblocks + localblock;;
            _localOffset = _x;
        }
        if ( _y > _endy) {
            quint32 newz = _z + (_y - _box.min_corner().y()) / _box.ylength();
            _currentBlock = newz * _grid->blocksPerBand() + _y / _grid->maxLines();
            _zChanged = newz != _z;
            _z = newz;
            _y = _box.min_corner().y() + (_y - _box.min_corner().y()) % _box.ylength();
            _yChanged = _y != tempy;
            localblock = _y /  _grid->maxLines();
            _localOffset = _y * _grid->size().xsize() + _x - localblock * _grid->maxLines() * _grid->size().xsize();
            if ( _z > _endz) { // done with this iteration block
                _linearposition = _endposition;
                return false;
            }
        }
    }
    return true;
}

inline bool PixelIterator::isAtEnd() const {
    return _x == _box.max_corner().x() &&
           _y == _box.max_corner().y() &&
           _z == _box.max_corner().z();
}

Voxel PixelIterator::position() const
{
    return Voxel(_x, _y, _z);
}

const Box3D<> &PixelIterator::box() const
{
    return _box;
}

quint32 PixelIterator::linearPosition() const
{
   // return sz.xsize() * sz.ysize() * _z + sz.xsize() * _y + _x;
    return _linearposition;
}

bool PixelIterator::move(int n) {

    bool ok = false;
    if (isAtEnd()) {
        _linearposition = _endposition;
        return false;
    }
    if ( _flow == fXYZ) {
        ok = moveXYZ(n);
    }
    else if ( _flow == fYXZ){
    }

    return ok;
}

PixelIterator& PixelIterator::operator=(const PixelIterator& iter) {
    copy(iter);
    return *this;
}

PixelIterator& PixelIterator::operator=(const PixelIterator&& iter) {
    copy(iter);
    return *this;
}

inline PixelIterator PixelIterator::operator++(int) {
    PixelIterator temp(*this);
    if(!move(1))
        return end();
    return temp;
}
inline PixelIterator PixelIterator::operator--(int) {
    PixelIterator temp(*this);
    move(-1);
    return temp;
}

bool PixelIterator::operator==(const PixelIterator& iter) const{
    return _linearposition == iter._linearposition;
}

bool PixelIterator::operator!=(const PixelIterator& iter) const{
    return ! operator ==(iter);
}

bool PixelIterator::operator<(const PixelIterator &iter) const
{
    return _linearposition < iter._linearposition;
}

bool PixelIterator::operator>(const PixelIterator &iter) const
{
    return _linearposition > iter._linearposition;
}

bool PixelIterator::operator<=(const PixelIterator &iter) const
{
    return _linearposition <= iter._linearposition;
}

bool PixelIterator::operator>=(const PixelIterator &iter) const
{
    return _linearposition >= iter._linearposition;
}

PixelIterator PixelIterator::end() const {
    PixelIterator iter(*this);
    iter += _endposition;
    return iter;
}

void PixelIterator::setFlow(Flow flw) {
    _flow = flw;
}

bool PixelIterator::contains(const Pixel& pix) {
    return pix.x() >= _box.min_corner().x() &&
            pix.x() < _box.max_corner().x()  &&
            pix.y() >= _box.min_corner().y() &&
            pix.y() < _box.max_corner().y();

}

bool PixelIterator::xchanged() const {
    return _xChanged;
}

bool PixelIterator::ychanged() const {
    return _yChanged;
}

bool PixelIterator::zchanged() const {
    return _zChanged;
}

void PixelIterator::initPosition() {
    const Size& sz = _raster->size();
    quint64 linpos = _y * sz.xsize() + _x;
    _currentBlock = _y / _grid->maxLines();
    _localOffset = linpos - _currentBlock * _grid->maxLines() * sz.xsize();
    _currentBlock += _z * _grid->blocksPerBand();
    _linearposition = sz.xsize() * sz.ysize() * _z + linpos;
    _endposition = sz.xsize() * sz.ysize() * sz.zsize();

}

int PixelIterator:: operator-(const PixelIterator& iter) {
    return linearPosition() - iter.linearPosition();
}


