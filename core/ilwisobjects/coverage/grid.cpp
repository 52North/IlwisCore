#include "raster.h"
#include "ilwiscontext.h"
#include "connectorinterface.h"
#include "geometries.h"
#include "grid.h"

using namespace Ilwis;
GridBlockInternal::GridBlockInternal(quint32 blocknr, quint64 rasterid,quint32 lines , quint32 width) :  _size(Size<>(width, lines,1)),_id(blocknr),_rasterid(rasterid), _initialized(false), _inMemory(false)
{
    _undef = undef<double>();
    _blockSize = _size.xsize()* _size.ysize();
}

GridBlockInternal::~GridBlockInternal() {

}

Size<> GridBlockInternal::size() const {
    return _size;
}

GridBlockInternal *GridBlockInternal::clone()
{
    GridBlockInternal *block = new GridBlockInternal(_id, i64UNDEF, _size.xsize(), _size.ysize());
    block->prepare();
    block->_undef = _undef;
    block->_blockSize = _blockSize;
    if(!inMemory())
        loadFromCache();
    else if (!_initialized){
        needData();
    }
    std::copy(_data.begin(), _data.end(), block->_data.begin());

    return block;


}

char *GridBlockInternal::blockAsMemory() {
    prepare();
    return (char *)&_data[0];
}

void GridBlockInternal::fill(const std::vector<double>& values) {

    if ( values.size() != _data.size())
        prepare(values.size() == 0);
    copy(values.begin(), values.end(), _data.begin());

}

quint32 GridBlockInternal::blockSize() {
    return _blockSize;
}

inline bool GridBlockInternal::save2Cache() {
    if ( !_inMemory) // nothing to do here
        return true;
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
    _tempName = _swapFile->fileName();
    quint64 bytesNeeded = _data.size() * sizeof(double);
    quint64 total =_swapFile->write((char *)&_data[0], bytesNeeded);
    _swapFile->close();
    if ( total != bytesNeeded) {
        return ERROR1(ERR_COULD_NOT_OPEN_WRITING_1,_tempName);
    }
    _initialized = false;
    _data = std::vector<double>();

    return true;

}

bool GridBlockInternal::loadFromCache() {
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

void GridBlockInternal::needData()
{
    IIlwisObject obj = mastercatalog()->get(_rasterid);
    if ( obj.isValid()){
        IRasterCoverage raster = obj.as<RasterCoverage>();
        raster->getData(_id);
    }
}


//----------------------------------------------------------------------
Grid::Grid(int maxlines) : _inMemoryIndex(iUNDEF), _memUsed(0),_blocksPerBand(0), _maxLines(maxlines){
    //Locker lock(_mutex);

    if ( _maxLines == iUNDEF){
         _maxLines = context()->configurationRef()("system-settings/grid-blocksize",500);
         if ( _maxLines > 1 && (_maxLines * size().xsize() * 8 > 1e7)) {
             _maxLines = max(1, 1e7 / (size().xsize() * 8));
         }
    }
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

    Grid *grid = new Grid(_maxLines);
    grid->prepare(0,Size<>(_size.xsize(), _size.ysize(), end - start));

    quint32 startBlock = start * _blocksPerBand;
    quint32 endBlock = std::min(end * _blocksPerBand, (quint32)_blocks.size());
    for(int i=startBlock, j=0; i < endBlock; ++i, ++j) {
        grid->_blocks[j] = _blocks[i]->clone();
    }
    grid->_inMemoryIndex = 1;
    grid->_memUsed = _memUsed;
    return grid;

}

void Grid::clear() {
    _size = Size<>();
    _blockSizes = std::vector<quint32>();
    for(quint32 i = 0; i < _blocks.size(); ++i) {
        delete _blocks[i];
    }
    _blocks = std::vector< GridBlockInternal *>();
    _cache = QList<quint32>();
    _blockSizes =  std::vector<quint32>();
    _offsets = std::vector<std::vector<quint32>>();
    _blockOffsets = std::vector<quint32>();
    _inMemoryIndex = iUNDEF;
    _allInMemory = false;
    _size = Size<>();

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

    Locker<> lock(_mutex); // slower case. must prevent other threads to messup admin
    if ( !_blocks[block]->inMemory())
      if(!update(block))
          throw ErrorObject(TR("Grid block is out of bounds"));

    return _blocks[block]->at(offset); // block is now in memory
}



inline void Grid::setValue(quint32 block, int offset, double v ) {
    if ( _allInMemory ) {
        _blocks[block]->at(offset) = v;
        return ;
    }

    Locker<> lock(_mutex);
    if ( !_blocks[block]->inMemory())
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
    if(!update(block, creation))
        return ;
    //block will be at front now
    _blocks[_cache[0]]->fill(data);
}

char *Grid::blockAsMemory(quint32 block, bool creation) {
    if ( _allInMemory) { // no cache case
        GridBlockInternal *du = _blocks[block];
        char * p = du->blockAsMemory();
        return p;
    }
    Locker<> lock(_mutex);
    if(!update(block, creation))
        return 0;
    GridBlockInternal *du = _blocks[_cache[block]];
    char * p = du->blockAsMemory();
    return p;

}


void Grid::setBandProperties(RasterCoverage *raster, int n){
    _size.zsize(_size.zsize() + n);
    quint32 oldBlocks = _blocks.size();
    quint32 newBlocks = numberOfBlocks();
    _blocks.resize(newBlocks);
    _blockSizes.resize(newBlocks);
    _blockOffsets.resize(newBlocks);
    qint32 totalLines = _size.ysize();

    for(quint32 block = oldBlocks; block < _blocks.size(); ++block) {
        int linesPerBlock = std::min((qint32)_maxLines, totalLines);
        _blocks[block] = new GridBlockInternal(block,raster ? raster->id() : i64UNDEF,linesPerBlock, _size.xsize());
        _blockSizes[block] = linesPerBlock * _size.xsize();
        _blockOffsets[block] = block == 0 ? 0 : _blockOffsets[block-1] +  _blockSizes[block];
        totalLines -= _maxLines;
        if ( totalLines <= 0) // to next band
            totalLines = _size.ysize();
    }
}

bool Grid::prepare(RasterCoverage *raster, const Size<> &sz) {
    Locker<> lock(_mutex);

    clear();
    _size = sz;

    if ( _size.isNull()|| !_size.isValid() || _maxLines == 0) {
        return false;
    }

    if ( _size.zsize() == 0)
        _size.zsize(1);

    if ( _maxLines > 1 && (_maxLines * sz.xsize() * 8 > 1e7)) {
        _maxLines = max(1, 1e7 / (sz.xsize() * 8));
    }

    quint64 bytesNeeded = _size.linearSize() * sizeof(double);
    quint64 mleft = context()->memoryLeft();
    if ( _memUsed != 0) // reszing a grid may reuse an older grid; in this case the memory has to be correctly given back
        context()->changeMemoryLeft(_memUsed);
    _memUsed = std::min(bytesNeeded, mleft/2);
    context()->changeMemoryLeft(-_memUsed);
    int n = numberOfBlocks();
    _inMemoryIndex = std::max(1ULL, n * _memUsed / bytesNeeded);
    _blocksPerBand = n / sz.zsize();


    int nblocks = numberOfBlocks();
    qint32 totalLines = _size.ysize();
    _blocks.resize(nblocks);
    _blockSizes.resize(nblocks);
    _blockOffsets.resize(nblocks);
    _allInMemory = _blocks.size() <= _inMemoryIndex;

    for(quint32 i = 0; i < _blocks.size(); ++i) {
        int linesPerBlock = std::min((qint32)_maxLines, totalLines);
        _blocks[i] = new GridBlockInternal(i, raster ? raster->id() : i64UNDEF,linesPerBlock, _size.xsize());
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
    if ( !_blocks[block]->inMemory()) { // if not loaded, load it from the temporary storage
        try{
        if(!_blocks[block]->loadFromCache()){
            return false;
        }
        } catch (const OutOfMemoryError& err){ // probably exceeded memory cappacity, unload the blocks and try again.
            unload(false);
            try {
                if(!_blocks[block]->loadFromCache()){
                    return false;
                }
            } catch (const OutOfMemoryError& err){
                throw OutOfMemoryError( TR("Couldnt allocate memory for raster"), true); // extreme memory-stress condition; this should never happen; the purpose of this catch-inside-catch and re-throw is to log the error in the issuelogger, so that we notice if it ever occurs
            }
        }
    }
    if ( creation || _cache.size() == 0) { // at create time we want to preserver the original order in memory

        if ( _cache.size() > 0 &&  block > _inMemoryIndex ){
            _blocks[_cache.back()]->save2Cache();
            _cache.removeLast();
        }
        _cache.push_front(block);
    }
    else if ( _cache.front() != block) {
        int index = _cache.indexOf(block);
        if (index >=0) { //  find the block and move it to the front of the list; blocks that are in use are at the front to prevent them being unloaded
            _cache.removeOne(index);
        }
        _cache.push_front(block); // block has now priority; if the block was not already in the list, the list will grow
        if ( index > (int)_inMemoryIndex) { // the index is bigger than allowed, so the last element of the list is unloaded
            if (_blocks[_cache.back()]->save2Cache()){
                _cache.removeLast();
                return true;
            }
        }

    }
    return true;
}

void Grid::unloadInternal(){
    _cache.clear();
    for(GridBlockInternal *block : _blocks) {
        block->save2Cache();
    }
    _inMemoryIndex = 0;
}

void Grid::unload(bool uselock) {
    if ( uselock) {
        Locker<> lock(_mutex);
        unloadInternal();
    }else
        unloadInternal();
}

std::map<quint32, std::vector<quint32> > Grid::calcBlockLimits(const IOOptions& options ){
    std::map<quint32, std::vector<quint32> > result;
    int blockplayer = blocksPerBand();
    if ( options.size() == 0){
        quint32 lastblock = 0;
        for(int layer = 0; layer < size().zsize(); ++layer){
            for(int block = 0; block < blockplayer; ++block){
                result[layer].push_back(lastblock + block);
            }
            lastblock += blockplayer;
        }
    }else {
        auto bindex = options["blockindex"];
        if ( bindex.isValid()){
            quint32 index = bindex.toUInt();
            int layer = index / blockplayer;
            result[layer].push_back(index);
        }

    }


    return result;

}

bool Grid::isValid() const
{
    return !(_size.isNull() || _size.isValid() || _inMemoryIndex == iUNDEF);
}

qint64 Grid::memUsed() const
{
    return _memUsed;
}



