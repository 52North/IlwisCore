#ifndef SIZE_H
#define SIZE_H

#include <QSize>
#include "kernel_global.h"

namespace Ilwis {

/*!
A 3D size measurer for integer spaces. It is similar to QSize of QT but then for 3 dimension. One can freely convert between QSize and Size
If no third dimension is present to top is assumed to be at 0. Fro consistency with QSize the top member is a signed integer though a unsigned would make more sense
The value of size dimensions can never be below 0 and will be reset to 0 if they do.
 */
template <typename T=quint32> class KERNELSHARED_EXPORT Size
{
public:
    /*!
    default constructor, x,y,top are assumed to be at 0,0,0
     */
    Size() : _xsize(0), _ysize(0), _zsize(0) {}
    /*!
    Initializes the Size with proper values
     * \param x x dimension of the Size
     * \param y y dimension of the Size
     * \param z z dimension of the Size, defaults to 1.
     */
    Size(T x, T y, T z) : _xsize(x), _ysize(y), _zsize(z) {}
    /*!
     Copy constructor
     * \param other Size
     */
    Size(const Size& sz) : _xsize(sz.xsize()), _ysize(sz.ysize()), _zsize(sz.zsize()) {}
    /*!
    Copy constructor from QSize, top goes to 0
     * \param other QSize
     */
    Size(const QSize& sz) : _xsize(sz.width()), _ysize(sz.height()), _zsize(1) {}
    /*!
    Adds a size to another Size. It extends the size with the values of the other Size
     * \param other Size
     * \return the new size as a reference
     */
    Size& operator+=(const Size& sz){
        if ( !isValid() ) {
            *this = Size();
            return *this;
        }

        _xsize += sz.xsize();
        _ysize += sz.ysize();
        _zsize += sz.zsize();
        check();
        return *this;
    }

    /*!
    Substract a size from another Size. It decreases the size with the values of the other Size
     * \param other Size
     * \return the new size as a reference
     */
    Size& operator-=(const Size& sz){
        if ( !isValid() ) {
            *this = Size();
            return *this;
        }

        _xsize -= sz.xsize();
        _ysize -= sz.ysize();
        _zsize -= sz.zsize();
        check();
        return *this;
    }

    /*!
    multiplies a size with a double. It decreases/increases all dimension values with the factor
     * \param multiplier
     * \return the new size as a reference
     */
    Size& operator*=(double f){
        if ( !isValid() || f == rUNDEF || f < 0) {
            *this = Size();
            return *this;
        }

        _xsize *= f;
        _ysize *= f;
        _zsize *= f;
        check();
        return *this;
    }

    /*!
     returns a QSize containing the x and y dimension values of the Size. Not often needed but were an auto cast is not working, this will give the same result
     * \return
     */
    QSize toQSize() const{
        return QSize(xsize(), ysize());
    }
    /*!
    returns the size in the x dimension
     * \return an 32-bits signed integers
     */
    T xsize() const{
        return _xsize;
    }
    /*!
    returns the size in the y dimension
     * \return an 32-bits signed integers
     */
    T ysize() const{
        return _ysize;
    }
    /*!
    returns the size in the z dimension
     * \return an 32-bits signed integers
     */
    T zsize() const{
        return _zsize;
    }

    Size<T> twod() const{
        return Size<T>(_xsize, _ysize,1);
    }

    QString toString() const{
        if ( !isValid())
            return sUNDEF;

        QString strsize = QString::number(_xsize) + " " + QString::number(_ysize);
        if ( _zsize > 1 && !isNumericalUndef(_zsize))
            strsize += " " + QString::number(_zsize);
        return strsize;
    }
    /*!
    returns the size in the x dimension as reference. So it can function as a setter
     * \return an 32-bits signed integers
     */
    void xsize(T x){
        _xsize = x;
    }

    /*!
    returns the size in the y dimension as reference. So it can function as a setter
     * \return an 32-bits signed integers
     */
    void ysize(T y){
        _ysize = y;
    }

    /*!
    returns the size in the z dimension as reference. So it can function as a setter
     * \return an 32-bits signed integers
     */
    void zsize(T z){
        _zsize = z;
    }

    /*!
    product of the 3 dimensions. It gives the total number of discrete points in the size.
    Be aware or rounding errors on float or double typed Size!
     * \return
     */
    quint64 linearSize() const{
        if (!isValid())
            return 0;
        return (quint64)xsize() * (quint64)ysize() * (quint64)zsize();
    }

    bool contains(T x, T y, T z=0) const{
        return x <= xsize() && y <= ysize() && z <= zsize();
    }

    bool isValid() const{
        if ( isNumericalUndef(_xsize) || isNumericalUndef(_ysize) || isNumericalUndef(_zsize)) // should not be possible but better safe then sorry
            return false;

        return _xsize >0 && _ysize > 0 && _zsize > 0;
    }
    /*!
     all dimensions are 0
     * \return true of all dimensions are 0
     */
    bool isNull() const{
        return _xsize == 0 && _ysize == 0 && _zsize == 0;
    }
private:
    void check(){
        {
            if ( _xsize < 0)
                xsize(0);
            if ( _ysize < 0)
                ysize(0);
            if (_zsize < 0)
                zsize(0);
        }
    }

    T _xsize;
    T _ysize;
    T _zsize;
    
};

//bool operator==(const Size& sz1, const Size& sz2) {
//    return sz1.xsize() == sz2.xsize() && sz1.ysize() == sz2.ysize() && sz1.zsize() == sz2.zsize();
//}

//bool operator!=(const Size& sz1, const Size& sz2) {
//    return ! operator==(sz1, sz2);
//}

template<typename T> Ilwis::Size<T> operator+(const Ilwis::Size<T>& sz1, const Ilwis::Size<T>& sz2){
    if((!sz1.isNull() && !sz1.isValid()) || (!sz2.isNull() && !sz2.isValid())){
        return Size<T>();
    }
    return Size<T>(sz1.xsize() + sz2.xsize(), sz1.ysize() + sz2.ysize(), sz1.zsize() + sz2.zsize());
}

template<typename T> Ilwis::Size<T> operator-(const Ilwis::Size<T>& sz1, const Ilwis::Size<T>& sz2){
    if((!sz1.isNull() && !sz1.isValid()) || (!sz2.isNull() && !sz2.isValid())){
        return Size<T>();
    }

    return Size<T>(sz1.xsize() - sz2.xsize(), sz1.ysize() - sz2.ysize(), sz1.zsize() - sz2.zsize());
}

template<typename T> bool operator==(const Ilwis::Size<T>& sz1, const Ilwis::Size<T>& sz2){
    if ( !sz1.isValid() && !sz2.isValid())
        return true;

    return sz1.xsize() == sz2.xsize() && sz1.ysize() == sz2.ysize() && sz1.zsize() == sz2.zsize();
}

template<typename T> bool operator!=(const Ilwis::Size<T>& sz1, const Ilwis::Size<T>& sz2){
    return !operator ==(sz1, sz2);
}
}
Q_DECLARE_METATYPE(Ilwis::Size<quint32>)
Q_DECLARE_METATYPE(Ilwis::Size<double>)

#endif // SIZE_H
