#include "kernel.h"
#include "raster.h"
#include "pixeliterator.h"


using namespace Ilwis;

bool PixelIterator::isValid() const {
    return _isValid;
}

PixelIterator::PixelIterator() : _localOffset(0), _currentBlock(0), _step(0), _flow(fXYZ),_isValid(false),_positionid(0) {

}

PixelIterator::PixelIterator(IGridCoverage raster, const Box3D<>& box, double step) :
    _raster(raster),
    _box(box),
    _localOffset(0),
    _currentBlock(0),
    _step(step),
    _flow(fXYZ),
    _isValid(false)
{
    init();
}

PixelIterator::PixelIterator(const PixelIterator& iter)  {
    copy(iter);
}

inline PixelIterator::PixelIterator(quint64 posid ) :
    _localOffset(0),
    _currentBlock(0),
    _step(0),
    _flow(fXYZ),
    _isValid(false),
    _positionid(posid)
{
}


void PixelIterator::copy(const PixelIterator &iter) {
    _raster = iter._raster;
    if ( _raster.isValid()) // TODO beyond end marker(end()) dont have a valid raster, no problem just yet but it maybe needed in the future
        _grid = _raster->_grid.data();
    _box = iter._box;
    _step = iter._step;
    _isValid = iter._isValid;
    _flow = iter._flow;
    _positionid = iter._positionid;
    _x = iter._x;
    _y = iter._y;
    _z = iter._z;
    _endx = iter._endx;
    _endy = iter._endy;
    _endz = iter._endz;
    _positionid = iter._positionid;
    _endpositionid = iter._endpositionid;
    _localOffset = iter._localOffset;
    _currentBlock = iter._currentBlock;

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

    initPosition();
    quint64 shift = _x + _y * 1e5 + (_z + 1) *1e10;
    _endpositionid = _positionid + shift;
    //_endpositionid = _endx + _endy * 1e5 + _endz*1e10 + _raster->id() * 1e13;
    _isValid = inside;
    _xChanged = _yChanged = _zChanged = false;
}

inline bool PixelIterator::moveXYZ(int n) {
    _x += n * _step;
    _localOffset += n;
    _xChanged = true;
    _yChanged = _zChanged = false;
    if ( _x > _endx) {
        ++_y;
        _x = _box.min_corner().x();
        _yChanged = true;
        _localOffset = _x + _y * _grid->size().xsize();
        if ( (_y % _grid->maxLines()) == 0) {
            ++_currentBlock;
            _localOffset = 0;
        }
        if ( _y > _endy) {
            ++_z;
            ++_currentBlock;
            _zChanged = true;
            _y = _box.min_corner().y();
            _localOffset = 0;
            if ( _z >= _endz) { // done with this iteration block
                _positionid = _endpositionid;
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

inline bool PixelIterator::move(int n) {
    if (isAtEnd()) {
        _positionid = _endpositionid;
        return false;
    }
    bool ok;
    if ( _flow == fXYZ) {
        ok = moveXYZ(n);
    }
    else if ( _flow == fYXZ){
    }
    if (ok)
        _positionid = calcPosId();

    return ok;
}

PixelIterator& PixelIterator::operator=(const PixelIterator& iter) {
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
    return _positionid == iter._positionid;
}

bool PixelIterator::operator!=(const PixelIterator& iter) const{
    return ! operator ==(iter);
}



PixelIterator PixelIterator::end() const {
    return PixelIterator(_endpositionid);
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
    quint64 linearPos = _y * sz.xsize() + _x;
    _currentBlock = _y / _grid->maxLines();
    _localOffset = linearPos - _currentBlock * _grid->maxLines() * sz.xsize();
    _currentBlock += _z * _grid->blocksPerBand();

    _positionid = calcPosId();
}


