#ifndef SIZE_H
#define SIZE_H

#include <QSize>
#include "Kernel_global.h"

namespace Ilwis {

/*!
A 3D size measurer for integer spaces. It is similar to QSize of QT but then for 3 dimension. One can freely convert between QSize and Size
If no third dimension is present to top is assumed to be at 0. Fro consistency with QSize the top member is a signed integer though a unsigned would make more sense
The value of size dimensions can never be below 0 and will be reset to 0 if they do.
 */
class KERNELSHARED_EXPORT Size : private QSize
{
public:
    /*!
    default constructor, x,y,top are assumed to be at 0,0,0
     */
    Size();
    /*!
    Initializes the Size with proper values
     * \param x x dimension of the Size
     * \param y y dimension of the Size
     * \param z z dimension of the Size, defaults to 1.
     */
    Size(qint32 x, qint32 y, qint32 z=1);
    /*!
     Copy constructor
     * \param other Size
     */
    Size(const Size& sz);
    /*!
    Copy constructor from QSize, top goes to 0
     * \param other QSize
     */
    Size(const QSize& sz);
    /*!
    Adds a size to another Size. It extends the size with the values of the other Size
     * \param other Size
     * \return the new size as a reference
     */
    Size& operator+=(const Size& sz);
    /*!
    Substract a size from another Size. It decreases the size with the values of the other Size
     * \param other Size
     * \return the new size as a reference
     */
    Size& operator-=(const Size& sz);
    /*!
    multiplies a size with a double. It decreases/increases all dimension values with the factor
     * \param multiplier
     * \return the new size as a reference
     */
    Size& operator*=(double f);
    /*!
     returns a QSize containing the x and y dimension values of the Size. Not often needed but were an auto cast is not working, this will give the same result
     * \return
     */
    QSize toQSize() const;
    /*!
    returns the size in the x dimension
     * \return an 32-bits signed integers
     */
    qint32 xsize() const;
    /*!
    returns the size in the y dimension
     * \return an 32-bits signed integers
     */
    qint32 ysize() const;
    /*!
    returns the size in the z dimension
     * \return an 32-bits signed integers
     */
    qint32 zsize() const;
    /*!
    returns the size in the x dimension as reference. So it can function as a setter
     * \return an 32-bits signed integers
     */
    void xsize(qint32 x);
    /*!
    returns the size in the y dimension as reference. So it can function as a setter
     * \return an 32-bits signed integers
     */
    void ysize(qint32 y);
    /*!
    returns the size in the z dimension as reference. So it can function as a setter
     * \return an 32-bits signed integers
     */
    void zsize(qint32 z);
    /*!
    product of the 3 dimension. It gives the total number of discreet points in the size
     * \return
     */
    quint64 totalSize() const;

    bool contains(qint32 x, qint32 y, qint32 z=0) const;
    bool isValid() const;
    /*!
     all dimensions are 0
     * \return true of all dimensions are 0
     */
    bool isNull() const;
private:
    void check() ;
    qint32 _top=0;
    
};

//bool operator==(const Size& sz1, const Size& sz2) {
//    return sz1.xsize() == sz2.xsize() && sz1.ysize() == sz2.ysize() && sz1.zsize() == sz2.zsize();
//}

//bool operator!=(const Size& sz1, const Size& sz2) {
//    return ! operator==(sz1, sz2);
//}

KERNELSHARED_EXPORT Ilwis::Size operator+(const Ilwis::Size& sz1, const Ilwis::Size& sz2);
KERNELSHARED_EXPORT Ilwis::Size operator-(const Ilwis::Size& sz1, const Ilwis::Size& sz2);
KERNELSHARED_EXPORT bool operator==(const Ilwis::Size& sz1, const Ilwis::Size& sz2);
KERNELSHARED_EXPORT bool operator!=(const Ilwis::Size& sz1, const Ilwis::Size& sz2);
}
Q_DECLARE_METATYPE(Ilwis::Size)

#endif // SIZE_H
