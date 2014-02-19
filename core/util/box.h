#ifndef BOX_H
#define BOX_H

#include <QSize>
#include "size.h"
#include "errmessages.h"
#include "range.h"

namespace Ilwis {
/*!
The box class is an abstraction for any rectangular area or volumne used in Ilwis. It is applicable for both
grid like areas ( e.g. rasters) as world coordinate areas.
 */
template<class PointType=Coordinate> class Box : public Range{
public:
    enum Dimension{dim0=0, dimX=1, dimY=2, dimZ=4};

    Box() : _min_corner(PointType(0,0,0)), _max_corner(PointType(0,0,0)){
    }

    Box(const PointType& pMin, const PointType& pMax) : _min_corner(pMin), _max_corner(pMax){
        normalize();
    }

    Box(const Box<PointType>& bx) : _min_corner(bx.min_corner()), _max_corner(bx.max_corner()) {

    }

    Box(Box<PointType>&& box) :
        _min_corner(std::move(box._min_corner)),
        _max_corner(std::move(box._max_corner))
    {
        box._min_corner = box._max_corner = PointType();
    }

    Box(const QSize& sz) : _min_corner(PointType(0,0,0)),_max_corner(PointType(sz.width()-1, sz.height()-1),0){
    }

    template<typename T> Box(const Size<T>& sz) : _min_corner(PointType(0,0,0)),_max_corner(PointType(sz.xsize()-1, sz.ysize()-1,sz.zsize()-1)){
    }

    /*!
     Constructs a box based on a WKT bases coordinate string
     * \param envelope, the coordinate string marked as POLYGON
     */
    Box(const QString& envelope) :  _min_corner(PointType(0,0)), _max_corner(PointType(0,0)){
        int index1 = envelope.indexOf("(");
        if ( index1 != -1) {
            int index2 = envelope.indexOf(")")    ;
            if ( index2 == -1){
                return;
            }

            QString coords = envelope.mid(index1+1, index2 - index1 - 1);
            coords = coords.trimmed();
            QStringList parts = coords.split(",");
            if ( parts.size() != 2){
                return;
            }
            QStringList p1 = parts[0].trimmed().split(' ');
            if ( p1.size() < 2)
                return;
            this->min_corner().x = p1[0].trimmed().toDouble();
            this->min_corner().y = p1[1].trimmed().toDouble();
            if ( p1.size() == 3)
                this->min_corner().z = p1[2].trimmed().toDouble();

            QStringList p2 = parts[1].trimmed().split(' ');
            if ( p1.size() < 2) {
                this->min_corner().x = 0;
                this->min_corner().y = 0;
                this->min_corner().z = 0;
                return;
            }
            this->max_corner().x = p2[0].trimmed().toDouble();
            this->max_corner().y = p2[1].trimmed().toDouble();
            if ( p2.size() == 3)
                this->max_corner().z = p2[2].trimmed().toDouble();
        }
    }

    IlwisTypes valueType() const{
        return max_corner().valuetype();
    }

    Range *clone() const{
        return new Box<PointType>(*this);
    }


    PointType min_corner() const {
        return _min_corner;
    }

    PointType max_corner() const {
        return _max_corner;
    }

    PointType& min_corner()  {
        return _min_corner;
    }

    PointType& max_corner()  {
        return _max_corner;
    }

    double xlength() const {
        return std::abs(this->min_corner().x - this->max_corner().x) + 1;
    }

    double ylength() const {
        return std::abs(this->min_corner().y - this->max_corner().y) + 1;
    }

    double zlength() const {
        return std::abs(this->min_corner().z - this->max_corner().z) + 1;
    }

    template<typename T=quint32> Size<T> size() const {
        return Size<T>(xlength(), ylength(), zlength());
    }

    bool is3D() const {
        return this->min_corner().is3D() && this->max_corner().is3D();
    }
    quint64 area() const {
        if ( !isValid())
            return 0;
        return xlength() * ylength();
    }

    quint64 volume() const {
        if (!is3D())
            return area();
        return xlength() * ylength() * zlength();
    }

    bool contains(const PointType& p) const {
        if (!p.isValid())
            return false;
        if(!isValid())
            return false;

        const PointType& pmin = this->min_corner();
        const PointType& pmax = this->max_corner();
        bool ok = p.x >= pmin.x && p.x <= pmax.x &&
                p.y >= pmin.y && p.y <= pmax.y;
        if ( is3D() && p.is3D()) {
            ok = p.z >= pmin.z && p.z <= pmax.z;
        }
        return ok;
    }

    bool contains(Box<PointType>& box) const{
        return contains(box.min_corner()) && contains(box.max_corner());
    }

    bool contains(const QVariant& value, bool inclusive = true) const {
        //TODO:
        return false;
    }

    bool equals(Box<PointType>& box, double delta=0) const {
        if ( !box.isValid())
            return false;
        if (!isValid())
            return false;

        const PointType& pmin = box.min_corner();
        const PointType& pmax = box.max_corner();

        if ( std::abs( min_corner.x - pmin.x) > delta)
            return false;
        if ( std::abs( min_corner.y - pmin.y) > delta)
            return false;
        if ( std::abs( max_corner.x - pmax.x) > delta)
            return false;
        if ( std::abs( max_corner.y - pmax.y) > delta)
            return false;
        if ( is3D() && box.is3D()) {
            if ( std::abs( min_corner.z - pmin.z) > delta)
                return false;
            if ( std::abs( max_corner.z - pmax.z) > delta)
                return false;
        }
        return true;
    }

    bool isValid() const {
        return this->min_corner().isValid() && this->max_corner().isValid();
    }

    bool isNull() const {
        bool ok = this->min_corner().x == 0 && this->min_corner().y == 0 &&
                this->max_corner().x == 0 && this->max_corner().y == 0;
        if ( is3D()){
            ok &= this->min_corner().z == 0 && this->max_corner().z == 0;
        }
        return ok;
    }

    Box<PointType>& operator=(Box<PointType>&& box) {
        _min_corner = std::move(box._min_corner);
        _max_corner = std::move(box._max_corner);

        box._min_corner = box._max_corner = PointType();
        return *this;
    }

    Box<PointType>& operator=(const Box<PointType>& box) {
        _min_corner = std::move(box._min_corner);
        _max_corner = std::move(box._max_corner);
        return *this;
    }

    Box<PointType>& operator +=(const double& v) {
        if ( isNumericalUndef(v))
            return *this;

        PointType& pmin = this->min_corner();
        PointType& pmax = this->max_corner();
        pmin -= v;
        pmax += v;
        normalize();
    }

    Box<PointType>& operator *=(const double& v) {
        if ( isNumericalUndef(v))
            return *this;
        PointType& pmin = this->min_corner();
        PointType& pmax = this->max_corner();
        double deltaX = xlength() * v / 2;
        double deltaY = ylength() * v / 2;
        double deltaZ = 1;
        if ( is3D())
            deltaZ = zlength() * v / 2;
        pmin *= {deltaX, deltaY, deltaZ};
        pmax *= {deltaX, deltaY, deltaZ};
        normalize();
    }

Box<PointType>& operator +=(const PointType& pnew) {
    if ( !pnew.isValid())
        return *this;



    PointType& pmin = this->min_corner();
    PointType& pmax = this->max_corner();
    if ( isNull() || !isValid()) {
        pmin = pnew;
        pmax = pnew;
        return *this;
    }

    if ( contains(pnew))
        return *this;
    if ( pmin.x > pnew.x)
        pmin.x  = pnew.x;
    if ( pmin.y > pnew.y)
        pmin.y = pnew.y;
    if ( pmax.x < pnew.x)
        pmax.x = pnew.x;
    if ( pmax.y < pnew.y)
        pmax.y = pnew.y;
    if ( is3D() && pnew.is3D()){
        if ( pmin.z > pnew.z)
            pmin.z  = pnew.z;
        if ( pmax.z < pnew.z)
            pmax.z = pnew.z;
    }
    normalize();

    return *this;

}

Box<PointType>& operator -=(const PointType& pnew) {
    if ( !pnew.isValid())
        return *this;

    PointType& pmin = this->min_corner();
    PointType& pmax = this->max_corner();

    if ( isNull() || !isValid()) {
        pmin = pnew;
        pmax = pnew;
        return *this;
    }

    if ( !contains(pnew))
        return *this;
    if ( pmin.x() < pnew.x())
        pmin.x = pnew.x();
    if ( pmin.y < pnew.y)
        pmin.y = pnew.y();
    if ( pmax.x > pnew.x)
        pmax.x  = pnew.x();
    if ( pmax.y > pnew.y)
        pmax.y = pnew.y();
    if ( is3D() && pnew.is3D()){
        if ( pmin.z < pnew.z)
            pmin.z  = pnew.z;
        if ( pmax.z > pnew.z)
            pmax.z = pnew.z;
    }
    normalize();

    return *this;

}

template<class T> Box<PointType>& operator +=(const std::vector<T>& vec) {
    int size = vec.size();
    if ( size == 2 || size == 3) {
        this->min_corner() += vec;
        this->max_corner() += vec;
        normalize();
    }

    return *this;
}

Box<PointType>& operator +=(const Box<PointType>& box) {
    if ( !box.isValid())
        return *this;

    operator+=(box.min_corner());
    operator+=(box.max_corner());
    return *this;
}

bool operator==(const Box<PointType>& box ) const {
    if ( !box.isValid())
        return false;

    return box.max_corner() == this->max_corner() && this->min_corner() == box.min_corner();
}

bool operator!=(const Box<PointType>& box ) const {
    return !(operator==(box));
}

QVariant impliedValue(const QVariant& v) const{
    QString type = v.typeName();
    bool ok = type == "Ilwis::Box<Pixel>" || type == "Ilwis::Box<Coordinate>" ||
            type == "Ilwis::Box<Pixeld>" ;
    if (!ok){
        return sUNDEF;
    }
    if ( type == "Ilwis::Box<Coordinate>"){
        Box<Coordinate> box = v.value<Box<Coordinate>>();
        return box.toString();
    }
    if ( type == "Ilwis::Box<Pixel>"){
        Box<Pixel> box = v.value<Box<Pixel>>();
        return box.toString();
    }
    if ( type == "Ilwis::Box<Pixeld>"){
        Box<Pixeld> box = v.value<Box<Pixeld>>();
        return box.toString();
    }
    return sUNDEF;

}

template<typename T> void ensure(const Size<T>& sz) {
    if ( xlength() > sz.xsize()) {
        this->max_corner().x  = sz.xsize() - 1 ;
    }
    if ( ylength() > sz.ysize()) {
        this->max_corner().y =  sz.ysize() - 1 ;
    }
    if ( zlength() > sz.zsize()) {
        this->max_corner().z =  sz.zsize() - 1 ;
    }
}

void copyFrom(const Box<PointType>& box, quint32 dimensions=dimX | dimY | dimZ) {
    if ( dimensions & dimX) {
        this->min_corner().x = box.min_corner().x;
        this->max_corner().x  =box.max_corner().x;
    }
    if ( dimensions & dimY) {
        this->min_corner().y = box.min_corner().y;
        this->max_corner().y = box.max_corner().y;
    }
    if ( dimensions & dimZ) {
        this->min_corner().z = box.min_corner().z;
        this->max_corner().z = box.max_corner().z;
    }
}


QString toString() const {
    if ( is3D()) {
        if (this->min_corner().valuetype() == itDOUBLE)
            return QString("POLYGON(%1 %2 %3,%4 %5 %6)").
                    arg((double)this->min_corner().x,0,'g').
                    arg((double)this->min_corner().y,0,'g').
                    arg((double)this->min_corner().z,0,'g').
                    arg((double)this->max_corner().x,0,'g').
                    arg((double)this->max_corner().y,0,'g').
                    arg((double)this->max_corner().z,0,'g');
        else
            return QString("POLYGON(%1 %2 %3,%4 %5 %6)").arg(this->min_corner().x).
                    arg(this->min_corner().y).
                    arg(this->min_corner().z).
                    arg(this->max_corner().x).
                    arg(this->max_corner().y).
                    arg(this->max_corner().z);


    }else {
        if (this->min_corner().valuetype() == itDOUBLE)
            return QString("POLYGON(%1 %2,%3 %4)").
                    arg((double)this->min_corner().x,0,'g').
                    arg((double)this->min_corner().y,0,'g').
                    arg((double)this->max_corner().x,0,'g').
                    arg((double)this->max_corner().y,0,'g');
        else
            return QString("POLYGON(%1 %2,%3 %4)").
                    arg(this->min_corner().x).
                    arg(this->min_corner().y).
                    arg(this->max_corner().x).
                    arg(this->max_corner().y);
    }

}

private:
    PointType _min_corner;
    PointType _max_corner;


void normalize() {
    PointType& pmin = this->min_corner();
    PointType& pmax = this->max_corner();
    if ( pmin.x > pmax.x) {
        double v1 = pmin.x;
        double v2 = pmax.x;
        std::swap(v1, v2);
        pmin.x  = v1;
        pmax.x = v2;

    }
    if ( pmin.y > pmax.y) {
        double v1 = pmin.y;
        double v2 = pmax.y;
        std::swap(v1, v2);
        pmin.y = v1;
        pmax.y = v2;
    }
    if ( pmin.z > pmax.z) {
        double v1 = pmin.z;
        double v2 = pmax.z;
        std::swap(v1, v2);
        pmin.z = v1;
        pmax.z = v2;
    }

}


};

template<typename PointType> Box<PointType> operator *(const Box<PointType>& box, const double& v) {
    PointType pmin = box.min_corner();
    PointType pmax = box.max_corner();
    double deltaX = box.xlength() * v / 2;
    double deltaY = box.ylength() * v / 2;
    double deltaZ = box.is3d() ? box.zlength() * v / 2 : 0;
    pmin -= {deltaX, deltaY, deltaZ};
    pmax += {deltaX, deltaY, deltaZ};
    return Box<PointType>(pmin, pmax);
}

typedef Ilwis::Box<Ilwis::Pixel> BoundingBox;
typedef Ilwis::Box<Ilwis::Coordinate> Envelope;

}


Q_DECLARE_METATYPE(Ilwis::BoundingBox)
Q_DECLARE_METATYPE(Ilwis::Box<Ilwis::Pixeld>)
Q_DECLARE_METATYPE(Ilwis::Envelope)



#endif // BOX_H
