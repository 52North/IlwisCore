#include <QDebug>
#include <QDir>
#include <QTemporaryFile>
#include <iostream>
#include "kernel.h"
#include "geometries.h"
#include "ilwiscontext.h"
#include "grid.h"

using namespace Ilwis;
GridBlockInternal::GridBlockInternal(quint32 lines , quint32 width) :  _size(Size(lines,width,1)),_initialized(false), _loaded(false)

{
    _undef = undef<double>();
    _id = ++_blockid;
    _blockSize = _size.xsize()* _size.ysize();
}

GridBlockInternal::~GridBlockInternal() {

}

Size GridBlockInternal::size() const {
    return _size;
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

bool GridBlockInternal::isLoaded() const {
    return _loaded;
}

inline bool GridBlockInternal::unload() {
    if ( _tempName == sUNDEF) {
        QString name = QString("gridblock_%1").arg(_id);
        QDir localDir(context()->temporaryWorkLocation().toLocalFile());
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
    _loaded = false;
    _initialized = false;
    _data.clear();

    return true;

}

bool GridBlockInternal::load() {
     _loaded = true;
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
        _loaded = false;
        return ERROR1(ERR_COULD_NOT_OPEN_READING_1,_tempName);
    }
    return true;

}


//----------------------------------------------------------------------
Grid::Grid(const Size& sz, int maxLines) : _maxLines(maxLines){
    //Locker lock(_mutex);

    setSize(sz);
    quint64 bytesNeeded = _size.totalSize() * sizeof(double);
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

Size Grid::size() const {
    return _size;
}

int Grid::maxLines() const
{
    return _maxLines;
}

void Grid::clear() {
    _size = Size();
    _blockSizes.clear();
    for(quint32 i = 0; i < _blocks.size(); ++i) {
        delete _blocks[i];
    }
    _blocks.clear();
}

double Grid::value(const Voxel& vox) {
    if ( vox.x() <0 || vox.y() < 0 || vox.z() < 0)
        return rUNDEF;
    quint32 yoff = (qint32)vox.y() % _maxLines;
    quint32 block = vox.y() / _maxLines;
    quint32 bandBlocks = _blocksPerBand * vox.z();
    quint32 offset = _offsets[yoff][vox.x()];
    return value(bandBlocks + block, offset);
}

inline double &Grid::value(quint32 block, int offset )  {
    if ( _blocks.size() <= _inMemoryIndex) // no cache case
        return _blocks[block]->at(offset);

    update(block);

    return _blocks[_cacheHead]->at(offset);
}



inline void Grid::setValue(quint32 block, int offset, double v ) {
    if ( _blocks.size() <= _inMemoryIndex) {
        _blocks[block]->at(offset) = v;
        return ;
    }

    if(!update(block))
        return ;

    _blocks[_cacheHead]->at(offset) = v;
}

quint32 Grid::blocks() const {
    return _blocks.size();
}

quint32 Grid::blocksPerBand() const {
    return _blocksPerBand;
}

void Grid::setBlock(quint32 block, const std::vector<double>& data, bool creation) {
    if ( _blocks.size() <= _inMemoryIndex) { // no cache case
        _blocks[block]->fill(data);
        return ;
    }
    if(!update(block, creation))
        return ;
    _blocks[_cache[block]]->fill(data);
}

char *Grid::blockAsMemory(quint32 block, bool creation) {
    if ( _blocks.size() <= _inMemoryIndex) { // no cache case
        GridBlockInternal *du = _blocks[block];
        char * p = du->blockAsMemory();
        return p;
    }

    if(!update(block, creation))
        return 0;
    GridBlockInternal *du = _blocks[_cache[block]];
    char * p = du->blockAsMemory();
    return p;

}

void Grid::setSize(const Size& sz) {
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
    Locker lock(_mutex);
    if ( block >= _blocks.size() )
        return false;
    if ( !_blocks[block]->isLoaded()) {
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
        if (index >=0) {
            _cache.removeOne(index);
        }
        _cache.push_front(block);
        _cacheHead = block; // for efficiency, no lookup what the head is
        if ( index > (int)_inMemoryIndex) {
            return _blocks[_cache[_inMemoryIndex]]->unload();
        }

    }
    return true;
}





