#ifndef GRID_H
#define GRID_H

#include "Kernel_global.h"

namespace Ilwis {

class BaseGrid {
public:
    BaseGrid() {}
    virtual ~BaseGrid() {}
    virtual double value(quint32 block, qint32 offset ) = 0;
    virtual double value(const Voxel& pix) = 0;
    virtual void setValue(quint32 block, qint32 offset, double value) = 0;
    virtual quint32 blockSize(quint32 index) const {
        if ( index < _blockSizes.size() )
            return _blockSizes[index];
        return iUNDEF;
    }
    virtual Size size() const { return _size;}
    virtual void setSize(const Size& sz) = 0;
    virtual quint32 blocks() const = 0;
    virtual void store(QDataStream &s, const SerializationOptions &opt) const = 0;
    virtual void setBlock(quint32 index, const std::vector<double>& data, bool creation) = 0;
    virtual char *blockAsMemory(quint32 index, bool creation) = 0;
    virtual quint32 blocksPerBand() const = 0;
    virtual bool prepare() = 0;

protected:
    virtual void clear() { _size = Size(); _blockSizes.clear(); }
    std::vector<quint32> _blockSizes;
    Size _size;
};

}

#endif // GRID_H
