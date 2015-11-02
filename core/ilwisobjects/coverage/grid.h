#ifndef Grid_H
#define Grid_H

#include <list>
#include <mutex>
#include <QDir>
#include <QTemporaryFile>
#include <iostream>
#include "kernel.h"
#include "errorobject.h"
#include "size.h"
#include "location.h"

namespace Ilwis {

class RasterCoverage;
struct IOOptions;

class GridBlockInternal {
public:
    GridBlockInternal(quint32 blocknr, quint64 rasterid, quint32 lines , quint32 width);
    ~GridBlockInternal();


    Size<> size() const ;
    GridBlockInternal *clone();

    double& at(quint32 index) {
        prepare();
        if ( index < _blockSize){
            return _data[index];
        }
        //throw ErrorObject(TR("Grid index out of bounds"));
        if (_undef != rUNDEF)
            _undef = rUNDEF;
        return _undef;
    }

    char *blockAsMemory();

    void fill(const std::vector<double>& values);

    quint32 blockSize();
    bool inMemory() const { return _inMemory; }
    inline bool save2Cache() ;
    bool loadFromCache();

private:
    void prepare(bool fetchData = true) {
        if (!_initialized) {
            Locker<> lock(_mutex);
            if ( _initialized) // may happen due to multithreading
                return;
            try{
            _data.resize(blockSize());
            std::fill(_data.begin(), _data.end(), _undef);
            _initialized = true;
            if (!inMemory() && _tempName != sUNDEF)
                loadFromCache();
            else if ( fetchData)
                needData();

            } catch(const std::bad_alloc& err){
                throw OutOfMemoryError( TR("Couldnt allocate memory for raster"), false);
            }


        }
    }

    void needData();
    std::recursive_mutex _mutex;
    std::vector<double> _data;
    double _undef;
    Size<> _size;
    quint64 _id;
    quint64 _rasterid;
    bool _initialized;
    bool _inMemory;
    QString _tempName = sUNDEF;
    QScopedPointer<QTemporaryFile> _swapFile;
    quint64 _blockSize;
};

class KERNELSHARED_EXPORT Grid

{
public:
    friend class GridInterpolator;

    Grid(int maxLines=iUNDEF);
    virtual ~Grid();

    void clear();

    //double value(const Point3D<double> &pix, int method=0);
    double& value(quint32 block, int offset );
    double value(const Pixel& pix) ;
    void setValue(quint32 block, int offset, double v );

    quint32 blocks() const;
    quint32 blocksPerBand() const;

    void setBlockData(quint32 block, const std::vector<double>& data, bool creation);
    char *blockAsMemory(quint32 block, bool creation);
    void setBandProperties(RasterCoverage *raster, int n);
    bool prepare(RasterCoverage *raster, const Size<> &sz) ;
    quint32 blockSize(quint32 index) const;
    Size<> size() const;
    int maxLines() const;
    Grid * clone(quint32 index1=iUNDEF, quint32 index2=iUNDEF) ;
    void unload(bool uselock=true);
    std::map<quint32, std::vector<quint32> > calcBlockLimits(const IOOptions &options);
    bool isValid() const;
    qint64 memUsed() const;
protected:

private:
    double bilinear(const Pixeld &pix) const;
    double bicubic(const Pixeld &pix) const;
    int numberOfBlocks();
    inline bool update(quint32 block, bool creation=false);
    void unloadInternal();


    std::recursive_mutex _mutex;
    std::vector< GridBlockInternal *> _blocks;
    QList<quint32> _cache;
    quint32 _inMemoryIndex;
    qint64 _memUsed;
    //quint64 _bandSize;
    quint32 _blocksPerBand;
    std::vector<quint32> _blockSizes;
    Size<> _size;
    quint32 _maxLines;
    std::vector<std::vector<quint32>> _offsets;
    std::vector<quint32> _blockOffsets;
    bool _allInMemory = false;

};

typedef std::unique_ptr<Grid> UPGrid;
}



#endif // Grid_H
