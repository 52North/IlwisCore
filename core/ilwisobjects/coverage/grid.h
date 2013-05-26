#ifndef Grid_H
#define Grid_H

#include "Kernel_global.h"
#include <list>
#include <mutex>


namespace Ilwis {

class GridBlockInternal{
public:
    GridBlockInternal(quint32 lines , quint32 width);
    ~GridBlockInternal();


    Size size() const ;

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
    bool isLoaded() const;
    inline bool unload() ;

    bool load();

private:
    void prepare() {
        if (!_initialized) {
            Locker lock(_mutex);
            if ( _initialized) // may happen due to multithreading
                return;
            _data.resize(blockSize());
            std::fill(_data.begin(), _data.end(), _undef);
            _initialized = true;
        }
    }
    std::mutex _mutex;
    std::vector<double> _data;
    double _undef;
    quint32 _index;
    Size _size;
    quint64 _id;
    bool _initialized;
    bool _loaded;
    static quint64 _blockid;
    QString _tempName = sUNDEF;
    QScopedPointer<QTemporaryFile> _swapFile;
    quint64 _blockSize;
};

class KERNELSHARED_EXPORT Grid

{
public:
    friend class GridInterpolator;

    Grid(const Size& sz, int maxLines=500);
    virtual ~Grid();

    void clear();

    //double value(const Point3D<double> &pix, int method=0);
    double& value(quint32 block, int offset );
    double value(const Voxel& pix) ;
    void setValue(quint32 block, int offset, double v );

    quint32 blocks() const;
    quint32 blocksPerBand() const;

    void setBlock(quint32 block, const std::vector<double>& data, bool creation);
    char *blockAsMemory(quint32 block, bool creation);
    void setSize(const Size& sz);
    bool prepare() ;
    quint32 blockSize(quint32 index) const;
    Size size() const;
    int maxLines() const;
private:
    double bilinear(const Point3D<double> &pix) const;
    double bicubic(const Point3D<double> &pix) const;
    int numberOfBlocks();
    inline bool update(quint32 block, bool creation=false);

    std::mutex _mutex;
    std::vector< GridBlockInternal *> _blocks;
    QList<quint32> _cache;
    quint32 _inMemoryIndex;
    qint64 _memUsed;
    quint64 _bandSize;
    quint32 _blocksPerBand;
    std::vector<quint32> _blockSizes;
    Size _size;
    quint32 _maxLines;
    quint32 _cacheHead =0;
    std::vector<std::vector<quint32>> _offsets;
    std::vector<quint32> _blockOffsets;
};
}



#endif // Grid_H
