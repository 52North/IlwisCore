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
    PixelIterator(const IRasterCoverage& raster, const Box3D<>& box=Box3D<>());
    PixelIterator(const PixelIterator& iter);
    PixelIterator(PixelIterator &&iter);

    PixelIterator& operator=(const PixelIterator& iter);
    PixelIterator& operator=(const PixelIterator&& iter);

    PixelIterator& operator++() {
        move(1);
        return *this;
    }
    PixelIterator& operator--() {
        move(-1);
        return *this;
    }

    PixelIterator& operator+=(int n) {
        move(n);
        return *this;
    }

    PixelIterator& operator-=(int n) {
        move(-n);
        return *this;
    }

    double& operator[](quint32 index){
        _x = 0;
        _y = 0;
        _z = 0;
        _yChanged = _xChanged = _zChanged = true;
        initPosition();
        move(index);
        return this->operator *();
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
    bool operator<(const PixelIterator& iter) const;
    bool operator<=(const PixelIterator& iter) const;
    bool operator>(const PixelIterator& iter) const;
    bool operator>=(const PixelIterator& iter) const;

    double& operator*() {
        return _grid->value(_currentBlock, _localOffset );
    }

    const double& operator*() const {
        return  _grid->value(_currentBlock, _localOffset);
    }

    double* operator->() {
        return &(_grid->value(_currentBlock, _localOffset ));
    }


    PixelIterator end() const ;

    void setFlow(Flow flw);
    bool contains(const Pixel& pix) ;
    bool xchanged() const;
    bool ychanged() const;
    bool zchanged() const;
    bool isAtEnd() const;
    Voxel position() const;
    const Box3D<>& box() const;
    quint32 linearPosition() const;
    int operator -(const PixelIterator &iter2);

    PixelIterator operator+(int n) {
        PixelIterator iter(*this);
        this->move(n);
        return iter;
    }

protected:
    PixelIterator(quint64 endpos ) :
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
        _linearposition(endpos),
        _endposition(endpos),
        _xChanged(false),
        _yChanged(false),
        _zChanged(false)
    {
    }

    void init();
    void initPosition();
    bool move(int n);
    bool moveXYZ(int delta) ;
    void copy(const PixelIterator& iter);

    IRasterCoverage _raster;
    Grid *_grid;
    Box3D<> _box;
    qint32 _x = 0;
    qint32 _y = 0;
    qint32 _z = 0;
    qint32 _localOffset = 0;
    qint32 _currentBlock = 0;
    Flow _flow;
    bool _isValid;
    qint32 _endx;
    qint32 _endy;
    qint32 _endz;
    quint64 _linearposition;
    quint64 _endposition;
    bool _xChanged =false;
    bool _yChanged = false;
    bool _zChanged = false;
};

inline Ilwis::PixelIterator begin(const IRasterCoverage& rasterCoverage) {
    return PixelIterator(rasterCoverage);
}

inline Ilwis::PixelIterator end(const IRasterCoverage& rasterCoverage) {
    PixelIterator iter(rasterCoverage);
    return iter.end();
}

}
#endif // PixelIterator_H
