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
    GridBlockInternal *clone(quint64 newRasterId);

    double& at(quint32 index) {
        if ( index < _blockSize){
            if (!_inMemory) {
                init();
                loadDiskDataToMemory();
            }
            return _data[index];
        } else
            return _undef;
    }

    char *blockAsMemory();
    void fill(const std::vector<double>& values);
    quint32 blockSize();
    bool inMemory() const { return _inMemory; }
    inline bool save2Cache() ;
    void dispose();
    void init();
    void loadDiskDataToMemory();

private:
    bool loadFromCache();
    void fetchFromSource();
    std::recursive_mutex _mutex;
    std::vector<double> _data;
    double _undef;
    Size<> _size;
    quint64 _id;
    quint64 _rasterid;
    bool _inMemory;
    QString _gridblockFileName = sUNDEF;
    QScopedPointer<QTemporaryFile> _gridblockFile;
    quint64 _blockSize;
};

class KERNELSHARED_EXPORT Grid

{
public:
    friend class GridInterpolator;

    Grid(int maxLines=iUNDEF);
    virtual ~Grid();

    void clear();

    double& value(quint32 block, int offset );
    double value(const Pixel& pix) ;
    void setValue(quint32 block, int offset, double v );

    quint32 blocks() const;
    quint32 blocksPerBand() const;

    void setBlockData(quint32 block, const std::vector<double>& data);
    char *blockAsMemory(quint32 block);
    void setBandProperties(RasterCoverage *raster, int n);
    bool prepare(RasterCoverage *raster, const Size<> &sz) ;
    quint32 blockSize(quint32 index) const;
    Size<> size() const;
    int maxLines() const;
    Grid * clone(quint64 newRasterId, quint32 index1=iUNDEF, quint32 index2=iUNDEF) ;
    void unload(bool uselock=true);
    std::map<quint32, std::vector<quint32> > calcBlockLimits(const IOOptions &options);
    bool isValid() const;
    qint64 memUsed() const;
protected:

private:
    int numberOfBlocks();
    inline bool update(quint32 block, bool loadDiskData);
    void unloadInternal();

    std::recursive_mutex _mutex;
    std::vector< GridBlockInternal *> _blocks;
    QList<quint32> _cache;
    quint32 _maxCacheBlocks;
    qint64 _memUsed;
    quint32 _blocksPerBand;
    std::vector<quint32> _blockSizes;
    Size<> _size;
    quint32 _maxLines;
    std::vector<std::vector<quint32>> _offsets;
    std::vector<quint32> _blockOffsets;
};

typedef std::unique_ptr<Grid> UPGrid;
}



#endif // Grid_H
