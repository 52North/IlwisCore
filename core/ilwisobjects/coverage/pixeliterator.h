#ifndef PixelIterator_H
#define PixelIterator_H



namespace Ilwis {

class Tranquilizer;

typedef std::shared_ptr<Tranquilizer> SPTranquilizer;

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
    bool isValid() const{
        return _isValid;
    }


    /*!
     * \brief PixelIterator The empty constructor for PixelIterator
     *
     * This constructor creates an empty and clean PixelIterator
     */
    PixelIterator();

    /*!
     * \brief PixelIterator Constructs a PixelIterator from a raster and a bounding box
     *
     * Constructs a PixelIterator from the raster and the bounding box
     * The bounding box is the area within the raster which should be walked by the PixelIterator
     * and it also calculates the rest of the internal members from these parameters
     * If one of the parameters was invalid the PixelIterator will also be invalid
     *
     * \param raster The raster from which this PixelIterator should be created
     * \param box The bounding box which desides what part of the raster should be walked
     */
    PixelIterator(const IRasterCoverage& raster, const Box3D<>& box=Box3D<>());

    /*!
     * Copy's all the values from the existing PixelIterator onto this one
     *
     * \param iter The iterator whose values should be duplicated
     */
    PixelIterator(const PixelIterator& iter);

    /*!
     * \brief PixelIterator move constructor
     * \param iter PixelIterator that must be moved
     */
    PixelIterator(PixelIterator &&iter);

    /*!
     * \brief operator = copy constructor
     * \param iter pixeliterator that must be copied
     * \return
     */
    PixelIterator& operator=(const PixelIterator& iter);

    /*!
     * \brief operator = move opperation
     * \param iter PixelIterator that must be moved
     * \return the new PixelIterator that is actually still the same
     */
    PixelIterator& operator=(const PixelIterator&& iter);

    /*!
     * \brief operator ++ adds 1 to this PixelIterator
     * \return This + 1
     */
    PixelIterator& operator++() {
        move(1);
        return *this;
    }

    /*!
     * \brief operator -- substracts 1 from this PixelIterator
     * \return this - 1
     */
    PixelIterator& operator--() {
        move(-1);
        return *this;
    }

    /*!
     * \brief operator += adds n to this PixelIterator
     * \param n amount to add to this PixelIterator
     * \return this + n
     */
    PixelIterator& operator+=(int n) {
        move(n);
        return *this;
    }

    /*!
     * \brief operator -= substracts n from this PixelIterator
     * \param n amount to substract
     * \return this - n
     */
    PixelIterator& operator-=(int n) {
        move(-n);
        return *this;
    }

    /*! //?//
     * \brief operator [] moves the PixelIterator to a certain index
     *
     * resets the position of thepixeliterator and than moves to the specified index
     *
     * \param index the target index
     * \return a PixelIterator at index
     */
    double& operator[](quint32 index){
        _x = 0;
        _y = 0;
        _z = 0;
        _yChanged = _xChanged = _zChanged = true;
        initPosition();
        move(index);
        return this->operator *();
    }

    /*! //?//
     * \brief operator () moves the PixelIterator to the given Voxel
     * moves the PixelIterator to the given Voxel, also adjusts the lineairposition
     * \param vox the Voxel to move to
     * \return this at the given Voxel
     */
    PixelIterator &operator ()(const Voxel &vox)
    {
        _x = vox.x();
        _y = vox.y();
        _z = vox.z();
        _yChanged = _xChanged = _zChanged = true;
        initPosition();
        return *this;
    }

    /*!
     * \brief operator ++ adds 1 to this PixelIterator
     * \return this + 1
     */
    PixelIterator operator++(int);

    /*!
     * \brief operator -- substracts 1 from this PixelIterator
     * \return this - 1
     */
    PixelIterator operator--(int);

    /*! //?//
     * \brief operator []
     * \param vox
     * \return
     */
    PixelIterator& operator[](const Voxel& vox);

    /*!
     * \brief operator == checks if this PixelIterator has the same linearPosition as another PixelIterator
     * \param iter another PixelIterator
     * \return true when linearPosition is equal
     */
    bool operator==(const PixelIterator& iter) const;

    /*!
     * \brief operator != Checks if this PixelIterator has a different linearPosition as another PixelIterator
     * \param iter another PixelIterator
     * \return true when linearPosition is not equal
     */
    bool operator!=(const PixelIterator& iter) const;

    /*!
     * \brief operator < Checks if this PixelIterator has a smaller linearPosition as another PixelIterator
     * \param iter another PixelIterator
     * \return true when linearPosition(this)<linearPosition(iter)
     */
    bool operator<(const PixelIterator& iter) const;

    /*!
     * \brief operator <= Checks if this PixelIterator has a smaller or equal linearPosition as another PixelIterator
     * \param iter another PixelIterator
     * \return true when linearPosition(this)<=linearPosition(iter)
     */
    bool operator<=(const PixelIterator& iter) const;

    /*!
     * \brief operator > Checks if this PixelIterator has a bigger linearPosition as another PixelIterator
     * \param iter another PixelIterator
     * \return true when linearPosition(this)>linearPosition(iter)
     */
    bool operator>(const PixelIterator& iter) const;

    /*!
     * \brief operator >= Checks if this PixelIterator has a bigger or equal linearPosition as another PixelIterator
     * \param iter another PixelIterator
     * \return true when linearPosition(this)>=linearPosition(iter)
     */
    bool operator>=(const PixelIterator& iter) const;

    /*!
     * \brief operator * Query for a reference to the current vallue of the PixelIterator
     * \return reference to the currentvalue
     */
    double& operator*() {
        return _grid->value(_currentBlock, _localOffset );
    }

    /*!
     * \brief operator * Query for a reference to the current vallue of the PixelIterator
     * \return reference to the currentvalue
     */
    const double& operator*() const {
        return  _grid->value(_currentBlock, _localOffset);
    }

    /*!
     * \brief operator -> Query for a pointer to the current value of the PixelIterator
     * \return ->value(this(current))
     */
    double* operator->() {
        return &(_grid->value(_currentBlock, _localOffset ));
    }

    /*!
     * \brief end returns the end position of this PixelIterator, this is 1 past the actual lastblock of the boundingbox
     * \return the endvalue of the lineairposition
     */
    PixelIterator end() const ;

    /*!
     * \brief setFlow changes the flow of this PixelIterator
     *
     * changes the flow according to the submitted value for flw,
     * if flw =fXYZ
     * this means that the pixeliterator will start at the first row in x direction,
     * at the end of this row, it will go 1 in y direction and start over in x direction,
     * at the end of the y row, it will go 1 in z direction and start over
     *
     * note at this moment only the xyz flow is implemented
     * \param flw the requested flow
     */
    void setFlow(Flow flw);

    /*!
     * \brief contains checks if a certain pixel is inside this PixelIterator
     * \param pix the pixel to be checked
     * \return true when the pixel is in this PixelIterator
     */
    bool contains(const Pixel& pix) ;

    /*!
     * \brief xchanged checks if the x coordinate has changed in the last step taken
     * \return true if the x has changed
     */
    bool xchanged() const;

    /*!
     * \brief ychanged checks if the y coordinate has changed in the last step taken
     * \return true if the y has changed
     */
    bool ychanged() const;

    /*!
     * \brief zchanged checks if the z coordinate has changed in the last step taken
     * \return true if the z has changed
     */
    bool zchanged() const;

    /*!
     * \brief isAtEnd checks if this PixelIterator is at its endpoint
     * \return true if it is at the end
     */
    bool isAtEnd() const;

    /*!
     * \brief position Query for the current position of this PixelIterator
     * \return a Voxel with the current position of this PixelIterator
     */
    Voxel position() const;

    /*!
     * \brief box Query for the bounding box of this PixelIterator
     * \return the bounding box of this PixelIterator
     */
    const Box3D<>& box() const;

    /*!
     * \brief linearPosition Query for the linearPosition of this PixelIterator
     * \return the lineairposition
     */
    quint32 linearPosition() const;

    /*!
     * \brief operator - substracts another pixeliterator fomr this PixelIterator
     * \param iter2 another PixelIterator
     * \return lineairposition(this)-lineairposition(iter2)
     */
    int operator -(const PixelIterator &iter2);

    /*!
     * \brief operator + adds n to this PixelIterator and returns it
     * \param n the amount to be added
     * \return this + n
     */
    PixelIterator operator+(int n) {
        PixelIterator iter(*this);
        this->move(n);
        return iter;
    }

    /*! //?//
     * \brief setTranquilizer gui stuffz
     * \param trq
     */
    void setTranquilizer(const SPTranquilizer& trq) {
        _trq = trq;
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
    SPTranquilizer _trq;
};

inline Ilwis::PixelIterator begin(const IRasterCoverage& raster) {
    return PixelIterator(raster);
}

inline Ilwis::PixelIterator end(const IRasterCoverage& raster) {
    PixelIterator iter(raster);
    return iter.end();
}

}
#endif // PixelIterator_H
