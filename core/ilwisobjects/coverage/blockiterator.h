#ifndef BLOCKITERATOR_H
#define BLOCKITERATOR_H

#include "ilwis.h"
#include "boost/geometry.hpp"

namespace Ilwis {

class BlockIterator;
class GridBlock;

class KERNELSHARED_EXPORT CellIterator : public std::iterator<std::random_access_iterator_tag, double> {
public:
    CellIterator(GridBlock *bl, quint32 start=0);

    CellIterator& operator=(CellIterator& iter);


    CellIterator& operator++() {
        move(1);
        return *this;
    }
    CellIterator& operator--() {
        move(-1);
        return *this;
    }

    CellIterator operator++(int);
    CellIterator operator--(int);

    double& operator*() ;

    qint32 position() const;
    Ilwis::Size blocksize() const;

private:
    void move(int n);
    GridBlock  *_block;
    qint32 _position;
};

bool operator==(const CellIterator& iter1, const CellIterator& iter2) {
    return iter1.blocksize() == iter2.blocksize() && iter1.position() == iter2.position();
}

bool operator!=(const CellIterator& iter1, const CellIterator& iter2) {
    return ! Ilwis::operator==(iter1, iter2);
}

class KERNELSHARED_EXPORT GridBlock {

public:
    GridBlock(BlockIterator& biter);
    double& operator()(quint32 x, quint32 y, quint32 z=0);
    Size size() const;
    CellIterator begin() ;
    CellIterator end() ;
private:
    BlockIterator& _iterator;
    std::vector<qint32> _internalBlockNumber;
    std::vector<qint32> _offsets;
    quint32 _blockYSize;
    quint32 _blockXSize;
};

class KERNELSHARED_EXPORT BlockIterator : protected PixelIterator {
public:
    friend class GridBlock;

    BlockIterator( IGridCoverage raster, const Size& sz, const Box3D<>& box=Box3D<>() , double step=1);

    GridBlock& operator*() {
        return _block;
    }
    BlockIterator& operator++();
    BlockIterator& operator--();
    BlockIterator end() const ;
    bool operator==(const BlockIterator& iter) const;
    bool operator!=(const BlockIterator& iter) const;
    Size blockSize() const;
private:
    BlockIterator(quint64 endpos);
    GridBlock _block;
    Size _blocksize;
    double _outside=rILLEGAL;
};


}

#endif // BLOCKITERATOR_H
