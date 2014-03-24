#include <QDebug>
#include <QDir>
#include <QTemporaryFile>
#include <iostream>
#include "kernel.h"
#include "geometries.h"
#include "ilwiscontext.h"
#include "grid.h"

using namespace Ilwis;
GridBlockInternal::GridBlockInternal(quint32 lines , quint32 width) :  _size(Size<>(width, lines,1)),_initialized(false), _inMemory(false)

{
    _undef = undef<double>();
    _id = ++_blockid;
    _blockSize = _size.xsize()* _size.ysize();
}

GridBlockInternal::~GridBlockInternal() {

}

Size<> GridBlockInternal::size() const {
    return _size;
}

GridBlockInternal *GridBlockInternal::clone()
{
    GridBlockInternal *block = new GridBlockInternal(_size.xsize(), _size.ysize());
    block->prepare();
    block->_undef = _undef;
    block->_index = 0;
    block->_blockSize = _blockSize;
    if(!isLoaded())
        load();
    std::copy(_data.begin(), _data.end(), block->_data.begin());

    return block;


}

char *GridBlockInternal::blockAsMemory() {
    prepare();
    return (char *)&_data[0];
}

void GridBlockInternal::fill(const std::vector<double>& values) {

    if ( values.size() != _data.size())
        prepare();
    copy(values.begin(), values.end(), _data.begin());

}

quint32 GridBlockInternal::blockSize() {
    return _blockSize;
}

inline bool GridBlockInternal::unload() {
    _inMemory = false;
    if ( _tempName == sUNDEF) {
        QString name = QString("gridblock_%1").arg(_id);
        QDir localDir(context()->cacheLocation().toLocalFile());
        if ( !localDir.exists()) {
            localDir.mkpath(localDir.absolutePath());
        }
        _tempName = localDir.absolutePath() + "/" + name;
    }
    if ( _swapFile.isNull()) {
        _swapFile.reset(new QTemporaryFile(_tempName));

    }
    if(!_swapFile->open() ){
        return ERROR1(ERR_COULD_NOT_OPEN_WRITING_1,_tempName);
    }
    quint64 bytesNeeded = _data.size() * sizeof(double);
    quint64 total =_swapFile->write((char *)&_data[0], bytesNeeded);
    _swapFile->close();
    if ( total != bytesNeeded) {
        return ERROR1(ERR_COULD_NOT_OPEN_WRITING_1,_tempName);
    }
    _initialized = false;
    _data.clear();

    return true;

}

bool GridBlockInternal::load() {
     _inMemory = true;
    prepare();
    if ( _tempName == sUNDEF) {
        return true; // totaly new block; never been swapped so no load needed
    }
    quint64 bytesNeeded = _data.size() * sizeof(double);
    if(!_swapFile->open() ){
        return ERROR1(ERR_COULD_NOT_OPEN_READING_1,_tempName);
    }
    quint64 total =_swapFile->read((char *)&_data[0], bytesNeeded);

    _swapFile->close();
    if ( total != bytesNeeded) {
        _inMemory = false;
        return ERROR1(ERR_COULD_NOT_OPEN_READING_1,_tempName);
    }
    return true;

}


//----------------------------------------------------------------------
Grid::Grid(const Size<> &sz, int maxLines) : _maxLines(maxLines){
    //Locker lock(_mutex);

    setSize(sz);
    quint64 bytesNeeded = _size.linearSize() * sizeof(double);
    quint64 mleft = context()->memoryLeft();
    _memUsed = std::min(bytesNeeded, mleft/2);
    context()->changeMemoryLeft(-_memUsed);
    int n = numberOfBlocks();
    _inMemoryIndex = std::max(1ULL, n * _memUsed / bytesNeeded);
    _blocksPerBand = n / sz.zsize();

}

quint32 Grid::blockSize(quint32 index) const {
    if ( index < _blockSizes.size() )
        return _blockSizes[index];
    return iUNDEF;
}

Grid::~Grid() {
    clear();
    context()->changeMemoryLeft(_memUsed);
}

Size<> Grid::size() const {
    return _size;
}

int Grid::maxLines() const
{
    return _maxLines;
}

Grid *Grid::clone(quint32 index1, quint32 index2)
{
    if ( index2 < index1){
        ERROR2(ERR_INVALID_INIT_FOR_2,TR("grid limits"),TR("clone grid"));
        return 0;
    }
    quint32 start = index1 == iUNDEF ? 0 : index1;
    quint32 end = index2 == iUNDEF ? _blocks.size() / _blocksPerBand : index2 + 1;

    Grid *grid = new Grid(Size<>(_size.xsize(), _size.ysize(), end - start), _maxLines);
    grid->prepare();

    quint32 startBlock = start * _blocksPerBand;
    quint32 endBlock = std::min(end * _blocksPerBand, _blocks.size());
    for(int i=startBlock, j=0; i < endBlock; ++i, ++j) {
        grid->_blocks[j] = _blocks[i]->clone();
    }
    grid->_inMemoryIndex = 1;
    grid->_memUsed = _memUsed;
    return grid;

}

void Grid::clear() {
    _size = Size<>();
    _blockSizes.clear();
    for(quint32 i = 0; i < _blocks.size(); ++i) {
        delete _blocks[i];
    }
    _blocks.clear();
}

double Grid::value(const Pixel &pix) {
    if (pix.x <0 || pix.y < 0 || pix.x >= _size.xsize() || pix.y >= _size.ysize() )
        return rUNDEF;
   if ( pix.is3D() && (pix.z < 0 || pix.z >= _size.zsize()))
        return rUNDEF;

    quint32 yoff = (qint32)pix.y % _maxLines;
    quint32 block = pix.y / _maxLines;
    quint32 bandBlocks = _blocksPerBand * (pix.is3D() ? pix.z : 0);
    quint32 offset = _offsets[yoff][pix.x];
    return value(bandBlocks + block, offset);
}

inline double &Grid::value(quint32 block, int offset )  {
    if ( _allInMemory ) // no loads needed
        return _blocks[block]->at(offset);

    Locker lock(_mutex); // slower case. must prevent other threads to messup admin
    if ( !_blocks[block]->isLoaded())
      if(!update(block))
          throw ErrorObject(TR("Grid block is out of bounds"));

    return _blocks[block]->at(offset); // block is now in memory
}



inline void Grid::setValue(quint32 block, int offset, double v ) {
    if ( _allInMemory ) {
        _blocks[block]->at(offset) = v;
        return ;
    }

    Locker lock(_mutex);
    if ( !_blocks[block]->isLoaded())
        if(!update(block))
            return ;

    _blocks[block]->at(offset) = v;
}

quint32 Grid::blocks() const {
    return _blocks.size();
}

quint32 Grid::blocksPerBand() const {
    return _blocksPerBand;
}

void Grid::setBlockData(quint32 block, const std::vector<double>& data, bool creation) {
    if ( _allInMemory) { // no cache case
        _blocks[block]->fill(data);
        return ;
    }
    Locker lock(_mutex);
    if(!update(block, creation))
        return ;
    _blocks[_cache[block]]->fill(data);
}

char *Grid::blockAsMemory(quint32 block, bool creation) {
    if ( _allInMemory) { // no cache case
        GridBlockInternal *du = _blocks[block];
        char * p = du->blockAsMemory();
        return p;
    }
    Locker lock(_mutex);
    if(!update(block, creation))
        return 0;
    GridBlockInternal *du = _blocks[_cache[block]];
    char * p = du->blockAsMemory();
    return p;

}

void Grid::setSize(const Size<>& sz) {
    if ( _blocks.size() != 0)
        clear();
    _size = sz;
    if ( _size.zsize() == 0)
        _size.zsize(1); // 1 grid at least in the grid;

}

bool Grid::prepare() {
    Locker lock(_mutex);
    if ( _size.isNull()|| !_size.isValid() || _maxLines == 0) {
        return ERROR0("Grid size not properly initialized");
    }

    int nblocks = numberOfBlocks();
    qint32 totalLines = _size.ysize();
    _blocks.resize(nblocks);
    _blockSizes.resize(nblocks);
    _blockOffsets.resize(nblocks);
    _allInMemory = _blocks.size() <= _inMemoryIndex;

    for(quint32 i = 0; i < _blocks.size(); ++i) {
        int linesPerBlock = std::min((qint32)_maxLines, totalLines);
        _blocks[i] = new GridBlockInternal(linesPerBlock, _size.xsize());
        _blockSizes[i] = linesPerBlock * _size.xsize();
        _blockOffsets[i] = i == 0 ? 0 : _blockOffsets[i-1] +  _blockSizes[i];
        totalLines -= _maxLines;
        if ( totalLines <= 0) // to next band
            totalLines = _size.ysize();
    }
    _offsets.resize(_maxLines);
    for(quint32 y=0; y < _maxLines; ++y) {
        _offsets[y].resize(_size.xsize());
        for(quint32 x=0; x < _size.xsize(); ++x) {
            quint64 linearPos = y * size().xsize() + x;
            _offsets[y][x] = linearPos;
        }
    }
    return true;
}

int Grid::numberOfBlocks() {
    double rblocks = (double)_size.ysize() / _maxLines;
    int nblocks = (int)rblocks;
    double rest = rblocks - nblocks;
    if ( rest >= (1.0 / ( _maxLines + 1))) {
        nblocks++;
    }
    return nblocks * _size.zsize();
}

inline bool Grid::update(quint32 block, bool creation) {
    if ( block >= _blocks.size() ) // illegal, blocknumber is outside the allowed range
        return false;
    if ( !_blocks[block]->isLoaded()) { // if not loaded, load it from the temporary storage
        if(!_blocks[block]->load())
            return false;
    }
    if ( creation || _cache.size() == 0) { // at create time we want to preserver the original order in memory

        if ( block > _inMemoryIndex )
            _blocks[_cache.back()]->unload();
        _cache.push_back(block);
    }
    else if ( _cache.front() != block) {
        int index = _cache.indexOf(block);
        if (index >=0) { //  find the block and move it to the front of the list; blocks that are in use are at the front to prevent them being unloaded
            _cache.removeOne(index);
        }
        _cache.push_front(block); // block has now priority; if the block was not already in the list, the list will grow
        if ( index > (int)_inMemoryIndex) { // the index is bigger than allowed, so the last element of the list is unloaded
            return _blocks[_cache[_inMemoryIndex]]->unload();
        }

    }
    return true;
}

void Grid::unload() {
    Locker lock(_mutex);
    _cache.clear();
    for(GridBlockInternal *block : _blocks) {
        block->unload();
    }
    _inMemoryIndex = 0;
}





