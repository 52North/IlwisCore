#include "raster.h"
#ifdef Q_OS_WIN
#include "geos/geom/Envelope.inl"
#endif
#include "tranquilizer.h"
#include "table.h"
#include "pixeliterator.h"
#include "bresenham.h"
#include "vertexiterator.h"


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


PixelIterator::PixelIterator(const IRasterCoverage& raster, geos::geom::Geometry* selection) :
    _raster(raster),
    _localOffset(0),
    _currentBlock(0),
    _flow(fXYZ),
    _isValid(false)
{
    //geos::geom::GeometryTypeId type = selection->getGeometryTypeId();
    const geos::geom::Envelope *env = selection->getEnvelopeInternal();
    Coordinate crdMin = Coordinate(env->getMinX(), env->getMinY());
    Coordinate crdMax = Coordinate(env->getMaxX(), env->getMaxY());
    _box = raster->georeference()->coord2Pixel(Box<Coordinate>(crdMin, crdMax));
    init();
    Bresenham algo(_raster->georeference());
    VertexIterator iter(selection);

    std::vector<Pixel> selectionPix = algo.rasterize(::begin(iter), ::end(iter));
    if ( selectionPix.size() == 0){
        _isValid = false;
        return;
    }
    _selectionPixels.resize(_raster->size().ysize());
    // we use  a vector of sets to store the final cleaned selection as sets are both ordered and have no duplicates,
    if ( selection->getGeometryTypeId() == geos::geom::GEOS_POLYGON || selection->getGeometryTypeId() == geos::geom::GEOS_POLYGON)
        cleanUp4PolyBoundaries(selectionPix);
    else{
        for(int i = 0; i < selectionPix.size(); ++i){
            _selectionPixels[selectionPix[i].y].push_back(selectionPix[i].x) ;
        }
    }

    _selectionIndex = 0;
}

PixelIterator::PixelIterator(const IRasterCoverage &raster, const BoundingBox& box, Flow flow) :
    _raster(raster),
    _box(box),
    _localOffset(0),
    _currentBlock(0),
    _flow(flow),
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
    _zChanged(iter._zChanged),
    _selectionPixels(iter._selectionPixels),
    _selectionIndex(iter._selectionIndex),
    _insideSelection (iter._insideSelection)


{
}

PixelIterator::PixelIterator(const PixelIterator& iter)  {
    copy(iter);
}


void PixelIterator::copy(const PixelIterator &iter) {
    _raster = iter._raster;
    if ( _raster.isValid())
        _grid = _raster->_grid.get();
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
    _selectionPixels  = iter._selectionPixels;
    _selectionIndex = iter._selectionIndex;
    _insideSelection = iter._insideSelection;

}

void PixelIterator::init() {
    const Size<>& sz = _raster->size();
    if ( _box.isNull()) {
        _box = BoundingBox(sz);
    }
    _box.ensure(sz);

    _x = _box.min_corner().x;
    _y = _box.min_corner().y;
    _z = _box.min_corner().z;
    if ( isNumericalUndef(_z))
        _z = 0;

    _endx = _box.max_corner().x;
    _endy = _box.max_corner().y;
    _endz = _box.max_corner().z;
    if ( isNumericalUndef(_endz))
        _endz = 0;

    _grid = _raster->gridRef().get();
    if ( _grid == 0) {
        _isValid = false;
        if (!isValid())
            throw ErrorObject(TR("Using invalid pixeliterator, are all data sources accessible?"));
    }

    initPosition();
    bool inside = contains(Pixel(_x,_y));
    _isValid = inside;
    _xChanged = _yChanged = _zChanged = false;
}

bool PixelIterator::moveXY(int delta){
    _zChanged = (_z - delta) %  (int)_box.zlength() != 0;
    qint32 tempx = _x + (_z - _box.min_corner().z) / _box.zlength();
    _z = _box.min_corner().z + (_z - _box.min_corner().z) % (int)_box.zlength();
    _xChanged = tempx != _x;
    std::swap(_x,tempx);
    if ( _x > _endx) {
        quint32 newy = _y + (_x - _box.min_corner().x) / _box.xlength();
        _yChanged = newy != _y;
        _y = newy;
        _x = _box.min_corner().x + (_x - _box.min_corner().x) % (int)_box.xlength();
        _xChanged = _x != tempx;
        if ( _y > _endy) { // done with this iteration block
            _linearposition = _endposition;
            return false;
        }
    }
    qint32 ylocal = _y % _grid->maxLines();
    _localOffset = _x + ylocal * _grid->size().xsize();
    _currentBlock = _z * _grid->blocksPerBand() + _y / _grid->maxLines();
    _linearposition = _x + _y * _grid->size().xsize() + _z * _grid->size().xsize() * _grid->size().ysize();
    return true;
}



bool PixelIterator::moveXZ(int delta)
{
    qint32 tempy = _y;
    qint32 tempx = _x;
    _y = _box.min_corner().y + ( _y - _box.min_corner().y) % (int)_box.ylength();
    _x += (tempy - _y) / _box.ylength();
    _xChanged = tempx != _x;
    qint32 ylocal = _y % _grid->maxLines();
    _localOffset = _x + ylocal * _grid->size().xsize();
    _linearposition = _x + _y * _grid->size().xsize() + _z * _grid->size().xsize() * _grid->size().ysize();

//    quint32 localblock = _y /  _grid->maxLines();
//    quint32 bandblocks = _grid->blocksPerBand() * _z;
//    if ( bandblocks + localblock != _currentBlock) {
//        _currentBlock = bandblocks + localblock;;
//        _localOffset = _x;
//    }
    move2NextBlock();

    if ( _x > _endx){
        quint32 newz = _z + (_x - _box.min_corner().x) / _box.xlength();
        _currentBlock = newz * _grid->blocksPerBand() + _y / _grid->maxLines();
        _zChanged = newz != _z;
        _z = newz;
        tempx = _x;
        _x = _box.min_corner().x + (_x - _box.min_corner().x) % (int)_box.xlength();
        _xChanged = _x != tempx;
        quint32 localblock = _y /  _grid->maxLines();
        _localOffset = _y * _grid->size().xsize() + _x - localblock * _grid->maxLines() * _grid->size().xsize();
        if ( _z > _endz) { // done with this iteration block
            _linearposition = _endposition;
            return false;
        }
    }
    return true;
}

bool PixelIterator::move2NextBlock() {
    quint32 localblock = _y /  _grid->maxLines();
    quint32 bandblocks = _grid->blocksPerBand() * _z;
    if ( bandblocks + localblock != _currentBlock) {
        _currentBlock = bandblocks + localblock;
        _localOffset = _x + (_y % _grid->maxLines()) * _grid->size().xsize() ; // needs to be revised for z-dimension change (formula is wrong when moving with -- from topleft of band2 to bottomright of band1)
    }
    if ( _currentBlock >= _grid->blocks()){
        _linearposition = _endposition;
        return false;
    }
    return true;
}

bool PixelIterator::moveYZ(int delta){
    qint32 tempy;
    if ( _x < _box.min_corner().x){

        int xdelta = _box.min_corner().x - _x;
        _x = _box.max_corner().x;
        if ( _endposition == _linearposition - delta) { // special case, we were at one beyond the end;
            tempy = _box.max_corner().y;
            --_z;
            --_currentBlock;
            _linearposition = _z * _box.size().linearSize() + _box.max_corner().y * _grid->size().xsize() + _box.max_corner().x + delta + 1; // +1 because we were at 1 beyound the end.
        } else
            tempy = _y  - 1 - xdelta /  _box.xlength() + (_x - _box.min_corner().x) / _box.xlength();

        _xChanged = delta %  (int)_box.xlength() != 0;
    }
    else{
        tempy = _y + (_x - _box.min_corner().x) / _box.xlength(); // with a big delta, y might jump more number of lines == xdistance / box xsize
        _xChanged = (_x - delta) %  (int)_box.xlength() != 0;
        _x = _box.min_corner().x + (_x - _box.min_corner().x) % (int)_box.xlength(); //  the modulo xlength gives the jump in one line
    }

    _yChanged = tempy != _y;
    std::swap(_y,tempy);
    qint32 ylocal = _y % _grid->maxLines();
    _localOffset = _x + ylocal * _grid->size().xsize();

//    quint32 localblock = _y /  _grid->maxLines();
//    quint32 bandblocks = _grid->blocksPerBand() * _z;

//    if ( bandblocks + localblock != _currentBlock) {
//        _currentBlock = bandblocks + localblock;;
//        _localOffset = _x;
//    }
    move2NextBlock();
    if ( _y > _endy) {
        quint32 newz = _z + (_y - _box.min_corner().y) / _box.ylength();
        _currentBlock = newz * _grid->blocksPerBand() + _y / _grid->maxLines();
        _zChanged = newz != _z;
        _z = newz;
        _y = _box.min_corner().y + (_y - _box.min_corner().y) % (int)_box.ylength();
        _yChanged = _y != tempy;
        quint32 localblock = _y /  _grid->maxLines();
        _localOffset = _y * _grid->size().xsize() + _x - localblock * _grid->maxLines() * _grid->size().xsize();
        if ( _z > _endz) { // done with this iteration block
            _linearposition = _endposition;
            return false;
        }
    }else if ( _y < _box.min_corner().y){
        _y = _box.max_corner().y;
        --_z;
        _zChanged = true;
         quint32 localblock = _y /  _grid->maxLines();
        _localOffset = _y * _grid->size().xsize() + _x - localblock * _grid->maxLines() * _grid->size().xsize();
        if ( _z < 0) {
            return false;
        }

    }
    return true;
}


Pixel PixelIterator::position() const
{
    return Pixel(_x, _y, _z);
}

const BoundingBox &PixelIterator::box() const
{
    return _box;
}

quint32 PixelIterator::linearPosition() const
{
   // return sz.xsize() * sz.ysize() * _z + sz.xsize() * _y + _x;
    return _linearposition;
}

void PixelIterator::setRaster(const IRasterCoverage &raster) {
    const BoundingBox box;

    _raster = raster;
    _box = box;
    _localOffset = 0;
    _currentBlock = 0;
    _flow = fXYZ;
    _isValid = false;

    init();
}

PixelIterator& PixelIterator::operator=(const PixelIterator& iter) {
    copy(iter);
    return *this;
}

PixelIterator& PixelIterator::operator=(const PixelIterator&& iter) {
    copy(iter);
    return *this;
}

QVariant PixelIterator::operator()(const QString &column)
{
    quint32 raw = operator *();
    return _raster->attributeTable()->cell(column, raw);
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
    iter += _endposition - _linearposition;
    return iter;
}

void PixelIterator::toEnd()
{
    _x = _box.min_corner().x;
    _z = _endz + 1;
    _y = _box.min_corner().y;
    _linearposition = _endposition;
    _localOffset = _endposition - _z * _grid->blocksPerBand();
}

void PixelIterator::setFlow(Flow flw) {
    _flow = flw;
}

bool PixelIterator::contains(const Pixel& pix) {
    return pix.x >= _box.min_corner().x &&
            pix.x < _box.max_corner().x  &&
            pix.y >= _box.min_corner().y &&
            pix.y < _box.max_corner().y;
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
    const Size<>& sz = _raster->size();
    quint64 linpos = _y * sz.xsize() + _x;
    quint64 endpos = _endy * sz.xsize() + _endx;
    _currentBlock = _y / _grid->maxLines();
    _localOffset = linpos - _currentBlock * _grid->maxLines() * sz.xsize();
    _currentBlock += _z * _grid->blocksPerBand();
    _linearposition = sz.xsize() * sz.ysize() * _z + linpos;
    _endposition = sz.xsize() * sz.ysize() * _endz + endpos + 1; // one past the last valid position
}

int PixelIterator:: operator-(const PixelIterator& iter) {
    return linearPosition() - iter.linearPosition();
}

const IRasterCoverage& PixelIterator::raster() const
{
    return _raster;
}


bool PixelIterator::move2NextSelection(int delta)
{
    if ( _selectionIndex  >= _selectionPixels[_y].size() - 1){ // this was the last boundary on this row
        _x = _endx + 1; // put x beyond the edge of the box so moveXY will trigger a y shift
        if(!moveYZ(delta))
            return false;

        if ( _y >= _selectionPixels.size() || _selectionPixels[_y].size() == 0 )
            return false;
        int xnew = _selectionPixels[_y][0];
        _linearposition += xnew - _box.min_corner().x;
        _localOffset += xnew - _box.min_corner().x;
        _selectionIndex = 0;
        _insideSelection = false; //  we are not yet in a selection
        if ( _selectionPixels[_y].size() > 0){
            _x = _selectionPixels[_y][0] - delta; // -delta because the next iteration will add delta again to it setting it exact at the edge again

        }
    }else{
        int xnew = _selectionPixels[_y][++_selectionIndex];
        _linearposition += xnew - _box.min_corner().x;
        _localOffset += xnew - _box.min_corner().x;
        _x = xnew;
        moveYZ(delta);
        _x -= 1;
    }
    return true;
}

void PixelIterator::cleanUp4PolyBoundaries(const std::vector<Pixel>& selectionPix)
{
    std::vector<std::set<qint32>> preselection(_selectionPixels.size());
    for(const Pixel& pixel : selectionPix){
        if ( (pixel.y >= 0) && (pixel.y < _selectionPixels.size())){
            preselection[pixel.y].insert(pixel.x);
        }
    }
    int y = 0;
    int ystart = iUNDEF;
    // getting rid of all unneeded pixels. adjacent pixels are not needed as you can't rasterize between them
    for(const auto &vec : preselection){
        std::set<qint32> cleaned;
        if ( vec.size() > 2){
            qint32 xold = iUNDEF;
            for(auto x : vec){
                if ( std::abs(x - xold) != 1) { // we have had a jump, so add the x to the list
                     cleaned.insert(x);
                }
                xold = x;
            }
            if ( cleaned.size() %2 == 1) // always size musat be even , can be  missed because of one long line of pixels
                cleaned.insert(xold);
        }else
            cleaned = vec;
        if ( cleaned.size() != 0 && ystart == iUNDEF){
            ystart = y;
        }
        _selectionPixels[y].resize(cleaned.size());
        std::copy(cleaned.begin(), cleaned.end(), _selectionPixels[y].begin() ); //copy pixels to output vector
        //TODO this is a stop gap to remove line with uneven number of points; must make a more general solution in the future
        if ( _selectionPixels[y].size() % 2 == 1){
            _selectionPixels[y].erase(_selectionPixels[y].begin() + _selectionPixels[y].size() / 2);
        }
        ++y;
    }
    if ( ystart != iUNDEF){
        _y = ystart;
        _x =  _selectionPixels[_y][0] - 1;
    }
}
