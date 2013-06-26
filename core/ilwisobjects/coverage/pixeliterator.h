#ifndef PixelIterator_H
#define PixelIterator_H



namespace Ilwis {

//class Polygon;

/*!
 * \brief The PixelIterator class an iterator class that iteratos over all the pixels in an grid (or subsection of it)
 *
 *The pixeliterator is the main access mechanism (together with the blockiterator) to pixels in 2D or 3D gridcoverages. Basically it sees the pixels as one long linear space and moves over it. The movement (flow) can have several directions
 *which resembles directions in the 'real' world. In the default case it moves first over the x dirdction, than the y and finally the z direction. But one could just as well first move in the z direction than x, than y. Internally these movements
 *are translated to offsets in the linear space. The pixel iterator obeys the normal rules for iterators in the STL and thus can be combined with the algorithms in this library.
 */
class KERNELSHARED_EXPORT PixelIterator  : public std::iterator<std::random_access_iterator_tag, double> {
public:

    enum Flow { fXYZ, fYXZ, fXZY, fYZX, fZXY, fZYX};

    /*!
     * \brief isValid tells if an iterator is in a valid state.
     *
     *An iterator is valid if bounding box and grid are a valid combination. The rest of the internal members of the iterator are calculated from this
     * \return true if the bounding box fits in the grid.
     */
    bool isValid() const;
    PixelIterator();
    PixelIterator(IGridCoverage raster, const Box3D<>& box=Box3D<>(), double step=1.0);
    PixelIterator(IGridCoverage raster, const Polygon& pol, const Ilwis::ICoordinateSystem &csyIn, double step=1.0);
    PixelIterator(const PixelIterator& iter);

    PixelIterator& operator=(const PixelIterator& iter);

    PixelIterator& operator++() {
        move(1);
        return *this;
    }
    PixelIterator& operator--() {
        move(-1);
        return *this;
    }

    PixelIterator &operator ()(const Voxel &vox)
    {
        _x = vox.x();
        _y = vox.y();
        _z = vox.z();
        _yChanged = _xChanged = _zChanged = true;
        initPosition();
        return *this;
    }

    PixelIterator operator++(int);
    PixelIterator operator--(int);
    PixelIterator& operator[](const Voxel& vox);

    bool operator==(const PixelIterator& iter) const;
    bool operator!=(const PixelIterator& iter) const;

    double& operator*() {
        return _grid->value(_currentBlock, _localOffset );
    }

    const double& operator*() const {
        return  _grid->value(_currentBlock, _localOffset);
    }

    PixelIterator end() const ;

    void setFlow(Flow flw);
    bool contains(const Pixel& pix) ;
    bool xchanged() const;
    bool ychanged() const;
    bool zchanged() const;
    bool isAtEnd() const;
    Voxel position() const;
protected:
    PixelIterator(quint64 posid );

    void init();
    void initPosition();
    bool move(int n);
    bool moveXYZ(int n) ;
    bool moveXYZByPol(int n) ;
    quint64 calcPosId() const {
        return _x + _y * 1e5 + _z*1e10 + _raster->id() * 1e13;
    }

    void copy(const PixelIterator& iter);

    IGridCoverage _raster;
    Grid *_grid;
    Box3D<> _box;
    qint32 _x = 0;
    qint32 _y = 0;
    qint32 _z = 0;
    qint32 _localOffset = 0;
    qint32 _currentBlock = 0;
    double _step;
    Flow _flow;
    bool _isValid;
    qint32 _endx;
    qint32 _endy;
    qint32 _endz;
    quint64 _positionid;
    quint64 _endpositionid;
    bool _xChanged =false;
    bool _yChanged = false;
    bool _zChanged = false;
    std::vector<std::vector<int>> _polboundayChanges;

};

}
#endif // PixelIterator_H
