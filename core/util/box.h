#ifndef BOX_H
#define BOX_H

#include <QSize>
#include "size.h"

namespace bg = boost::geometry;

namespace Ilwis {
/*!
The box class is an abstraction for any rectangular area or volumne used in Ilwis. It is applicable for both
grid like areas ( e.g. rasters) as world coordinate areas.
 */
template<class CsyType=qint32> class Box2D : public bg::model::box<Ilwis::Point2D<CsyType> > {
public:
    Box2D() : bg::model::box<Ilwis::Point2D<CsyType> >(Ilwis::Point2D<CsyType>(0,0),Ilwis::Point2D<CsyType>(0,0)){
    }

    Box2D(const Ilwis::Point2D<CsyType>& pMin, const Ilwis::Point2D<CsyType>& pMax) : bg::model::box<Ilwis::Point2D<CsyType> >(pMin,pMax){
        normalize();
    }

    Box2D(const Box2D<CsyType>& bx) : bg::model::box<Ilwis::Point2D<CsyType> >(bx.min_corner(), bx.max_corner()) {

    }

    Box2D(const QSize& sz) : bg::model::box<Ilwis::Point2D<qint32> >(Point2D<qint32>(0,0),Point2D<qint32>(sz.width(), sz.height())){
    }

    Box2D(const Size& sz) : bg::model::box<Ilwis::Point2D<qint32> >(Point2D<qint32>(0,0),Point2D<qint32>(sz.xsize(), sz.ysize())){
    }

    /*!
     Constructs a box based on a WKT bases coordinate string
     * \param envelope, the coordinate string marked as POLYGON
     */
    Box2D(const QString& envelope) : bg::model::box<Ilwis::Point2D<CsyType> >(Point2D<CsyType>(0,0),Point2D<CsyType>(0,0)){
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
            this->min_corner().x((CsyType)p1[0].trimmed().toDouble());
            this->min_corner().y((CsyType)p1[1].trimmed().toDouble());
            QStringList p2 = parts[1].trimmed().split(' ');
            if ( p1.size() < 2) {
                this->min_corner().x(0);
                this->min_corner().y(0);
                return;
            }
            this->max_corner().x((CsyType)p2[0].trimmed().toDouble());
            this->max_corner().y((CsyType)p2[1].trimmed().toDouble());
        }
    }

    CsyType width() const {
        return std::abs(this->min_corner().x() - this->max_corner().x());
    }

    CsyType height() const {
        return std::abs(this->min_corner().y() - this->max_corner().y());
    }

    QSize size() const {
        return QSize(width(), height());
    }

    bool contains(const Ilwis::Point2D<CsyType>& p) const {
        const Ilwis::Point2D<CsyType>& pmin = this->min_corner();
        const Ilwis::Point2D<CsyType>& pmax = this->max_corner();
        return p.x() >= pmin.x() && p.x() <= pmax.y() &&
                p.y() >= pmin.y() && p.y() <= pmax.y();
    }

    bool contains(Box2D<CsyType>& box) const{
        return contains(box.min_corner()) && contains(box.max_corner());
    }

    bool isValid() const {
        return this->min_corner().isValid() && this->max_corner().isValid();
    }

    bool isNull() const {
        return this->width() == 0 && this->height() == 0;
    }

    Box2D<CsyType>& operator +=(const CsyType& v) {
        Ilwis::Point2D<CsyType>& pmin = this->min_corner();
        Ilwis::Point2D<CsyType>& pmax = this->max_corner();
        pmin -= v;
        pmax += v;
        normalize();
    }

    Box2D<CsyType>& operator -=(const CsyType& v) {
        Ilwis::Point2D<CsyType>& pmin = this->min_corner();
        Ilwis::Point2D<CsyType>& pmax = this->max_corner();
        pmin += v;
        pmax -= v;
        normalize();

    }

    Box2D<CsyType>& operator *=(const double& v) {
        Ilwis::Point2D<CsyType>& pmin = this->min_corner();
        Ilwis::Point2D<CsyType>& pmax = this->max_corner();
        double deltaW = width() * v / 2;
        double deltaH = height() * v / 2;
        pmin -= {deltaW, deltaH};
    pmax += {deltaW, deltaH};
normalize();
}

Box2D<CsyType>& operator +=(const Ilwis::Point2D<CsyType>& pnew) {
    Ilwis::Point2D<CsyType>& pmin = this->min_corner();
    Ilwis::Point2D<CsyType>& pmax = this->max_corner();
    if ( contains(pnew))
        return *this;
    if ( pmin.x() > pnew.x())
        pmin.x(pnew.x());
    if ( pmin.y() > pnew.y())
        pmin.y( pnew.y());
    if ( pmax.x() < pnew.x())
        pmax.x(pnew.x());
    if ( pmax.y() < pnew.y())
        pmax.y( pnew.y());
    normalize();

    return *this;

}
Box2D<CsyType>& operator -=(const Ilwis::Point2D<CsyType>& pnew) {
    Ilwis::Point2D<CsyType>& pmin = this->min_corner();
    Ilwis::Point2D<CsyType>& pmax = this->max_corner();
    if ( !contains(pnew))
        return *this;
    if ( pmin.x() < pnew.x())
        pmin.d1(pnew.x());
    if ( pmin.y() < pnew.y())
        pmin.d2( pnew.y());
    if ( pmax.x() > pnew.x())
        pmax.d1(pnew.x());
    if ( pmax.y() > pnew.y())
        pmax.d2( pnew.y());
    normalize();

    return *this;

}

Box2D<CsyType>& operator +=(std::initializer_list<CsyType> il) {
    int size = il.size();
    if ( size == 2) {
        Ilwis::Point2D<CsyType>& pmin = this->min_corner();
        Ilwis::Point2D<CsyType>& pmax = this->max_corner();
        pmin += {*(il.begin()), *(il.begin() + 1)};
    pmax += {*(il.begin()), *(il.begin() + 1)};
normalize();
}

return *this;
}

Box2D<CsyType>& operator *=(std::initializer_list<CsyType> il) {
    int size = il.size();
    if ( size == 2) {
        Ilwis::Point2D<CsyType>& pmin = this->min_corner();
        Ilwis::Point2D<CsyType>& pmax = this->max_corner();
        pmin *= {*(il.begin()), *(il.begin() + 1)};
    pmax *= {*(il.begin()), *(il.begin() + 1)};
normalize();
}

return *this;
}

bool operator==(const Box2D<CsyType>& box ) const {
    return box.max_corner() == this->max_corner() && this->min_corner() == box.min_corner();
}

bool operator!=(const Box2D<CsyType>& box ) const {
    return !(operator==(box));
}

QString toString() const {
    if (std::is_floating_point<CsyType>::value)
        return QString("POLYGON(%1 %2,%3 %4)").arg(this->min_corner().x(),0,'f').
                arg(this->min_corner().y(),0,'f').
                arg(this->max_corner().x(),0,'f').
                arg(this->max_corner().y(),0,'f');
    else
        return QString("POLYGON(%1 %2,%3 %4)").arg(this->min_corner().x()).
                arg(this->min_corner().y()).
                arg(this->max_corner().x()).
                arg(this->max_corner().y());

}

void store(QDataStream &s, const SerializationOptions &opt) const{

    this->min_corner().store(s, opt);
    this->max_corner().store(s, opt);
}

void load(QDataStream &s){
    return s;
}

private:
void normalize() {
    Ilwis::Point2D<CsyType>& pmin = this->min_corner();
    Ilwis::Point2D<CsyType>& pmax = this->max_corner();
    if ( pmin.x() > pmax.x()) {
        CsyType v1 = pmin.x();
        CsyType v2 = pmax.x();
        std::swap(v1, v2);
        pmin.x(v1);
        pmax.x(v2);

    }
    if ( pmin.y() > pmax.y()) {
        CsyType v1 = pmin.y();
        CsyType v2 = pmax.y();
        std::swap(v1, v2);
        pmin.y(v1);
        pmax.y(v2);
    }

}


};

template<class CsyType=qint32> class Box3D : public bg::model::box<Ilwis::Point3D<CsyType> > {
public:
    Box3D() : bg::model::box<Ilwis::Point3D<CsyType> >(Ilwis::Point3D<CsyType>(0,0,0),Ilwis::Point3D<CsyType>(0,0,0)){
    }

    Box3D(const Ilwis::Point3D<CsyType>& pMin, const Ilwis::Point3D<CsyType>& pMax) : bg::model::box<Ilwis::Point3D<CsyType> >(pMin,pMax){
        normalize();
    }

    Box3D(const Box3D<CsyType>& bx) : bg::model::box<Ilwis::Point3D<CsyType> >(bx.min_corner(), bx.max_corner()) {

    }

    Box3D(const Size& sz) : bg::model::box<Ilwis::Point3D<qint32> >(Point3D<qint32>(0,0,0),Point3D<qint32>(sz.xsize()-1, sz.ysize()-1, sz.zsize()-1)){
    }

    Box3D(const QString& envelope) : bg::model::box<Ilwis::Point3D<CsyType> >(Point3D<CsyType>(0,0,0),Point3D<CsyType>(0,0,0)){
        int index1 = envelope.indexOf("(");
        if ( index1 != -1) {
            int index2 = envelope.indexOf(")")    ;
            if ( index2 == -1){
                return;
            }

            QString coords = envelope.mid(index1+1, index2 - index1 - 1);
            coords = coords.trimmed();
            QStringList parts = coords.split(",");
            if ( (parts.size() == 2 || parts.size() == 3) == false){
                return;
            }
            QStringList p1 = parts[0].trimmed().split(' ');
            if ( p1.size() < 2)
                return;
            this->min_corner().x((CsyType)p1[0].trimmed().toDouble());
            this->min_corner().y((CsyType)p1[1].trimmed().toDouble());
            if ( p1.size() == 3)
                this->min_corner().z((CsyType)p1[2].trimmed().toDouble());

            QStringList p2 = parts[1].trimmed().split(' ');
            if ( p2.size() < 2) {
                 return;
            }
            this->max_corner().x((CsyType)p2[0].trimmed().toDouble());
            this->max_corner().y((CsyType)p2[1].trimmed().toDouble());
            if ( p2.size() == 3)
                this->min_corner().z((CsyType)p2[2].trimmed().toDouble());
        }
    }

    Box3D(const Box2D<CsyType>& box) : bg::model::box<Ilwis::Point3D<CsyType> >(box.min_corner(), box.max_corner()){
        this->min_corner().z(0);
        this->max_corner().z(0);
    }

    operator Box2D<CsyType> (){
        return Box2D<CsyType>(Point2D<CsyType>(this->min_corner().x(),
                                              this->min_corner().y()),
                             Point2D<CsyType>(this->max_corner().x(),
                                              this->max_corner().y()));
    }

    void ensure(const Size& sz) {
        if ( xlength() > sz.xsize()) {
            this->max_corner().x( sz.xsize() - 1 );
        }
        if ( ylength() > sz.ysize()) {
            this->max_corner().y( sz.ysize() - 1 );
        }
        if ( zlength() > sz.zsize()) {
            this->max_corner().z( sz.zsize() - 1 );
        }
    }

    CsyType xlength() const {
        return std::abs(this->min_corner().x() - this->max_corner().x()) + 1;
    }

    CsyType ylength() const {
        return std::abs(this->min_corner().y() - this->max_corner().y()) + 1;
    }

    CsyType zlength() const {
        return std::abs(this->min_corner().z() - this->max_corner().z()) + 1;
    }

    Size size() const {
        if ( !isValid())
            return Size(0,0,0);

        return Size(xlength(), ylength(), zlength());
    }

    bool isNull() const {
        return this->xlength() == 0 && this->ylength() == 0 && this->zlength() == 0;
    }

    bool contains(const Ilwis::Point3D<CsyType>& p) const {
        const Ilwis::Point3D<CsyType>& pmin = this->min_corner();
        const Ilwis::Point3D<CsyType>& pmax = this->max_corner();
        return p.x() >= pmin.x() && p.x() <= pmax.y() &&
                p.y() >= pmin.y() && p.y() <= pmax.y() &&
                p.z() >= pmin.z() && p.z() <= pmax.z();
    }

    bool contains(const Box3D<CsyType>& box) const{
        return contains(box.min_corner()) && contains(box.max_corner());
    }

    bool isValid() const {
        return this->min_corner().isValid() && this->max_corner().isValid();
    }

    Box3D<CsyType>& operator +=(const CsyType& v) {
        Ilwis::Point3D<CsyType>& pmin = this->min_corner();
        Ilwis::Point3D<CsyType>& pmax = this->max_corner();
        pmin -= v;
        pmax += v;
        normalize();
    }

    Box3D<CsyType>& operator -=(const CsyType& v) {
        Ilwis::Point3D<CsyType>& pmin = this->min_corner();
        Ilwis::Point3D<CsyType>& pmax = this->max_corner();
        pmin += v;
        pmax -= v;
        normalize();

    }

    Box3D<CsyType>& operator *=(const double& v) {
        Ilwis::Point3D<CsyType>& pmin = this->min_corner();
        Ilwis::Point3D<CsyType>& pmax = this->max_corner();
        double deltaX = xlength() * v / 2;
        double deltaY = ylength() * v / 2;
        double deltaZ = zlength() * v / 2;
        pmin -= {deltaX, deltaY, deltaZ};
        pmax += {deltaX, deltaY, deltaZ};
        normalize();
}

Box3D<CsyType>& operator +=(const Ilwis::Point3D<CsyType>& pnew) {
    Ilwis::Point3D<CsyType>& pmin = this->min_corner();
    Ilwis::Point3D<CsyType>& pmax = this->max_corner();
    if ( contains(pnew))
        return *this;
    if ( pmin.x() > pnew.x())
        pmin.x(pnew.x());
    if ( pmin.y() > pnew.y())
        pmin.y( pnew.y());
    if ( pmin.z() > pnew.z())
        pmin.z( pnew.z());
    if ( pmax.x() < pnew.x())
        pmax.x(pnew.x());
    if ( pmax.y() < pnew.y())
        pmax.y( pnew.y());
    if ( pmax.z() < pnew.z())
        pmax.z( pnew.z());
    normalize();

    return *this;

}
Box3D<CsyType>& operator -=(const Ilwis::Point2D<CsyType>& pnew) {
    Ilwis::Point3D<CsyType>& pmin = this->min_corner();
    Ilwis::Point3D<CsyType>& pmax = this->max_corner();
    if ( !contains(pnew))
        return *this;
    if ( pmin.x() < pnew.x())
        pmin.d1(pnew.x());
    if ( pmin.y() < pnew.y())
        pmin.y( pnew.y());
    if ( pmin.z() < pnew.z())
        pmin.z( pnew.z());
    if ( pmax.x() > pnew.x())
        pmax.x(pnew.x());
    if ( pmax.y() > pnew.y())
        pmax.y( pnew.y());
    if ( pmax.z() > pnew.z())
        pmax.z( pnew.z());
    normalize();

    return *this;

}

Box3D<CsyType>& operator +=(std::initializer_list<CsyType> il) {
    int size = il.size();
    if ( size == 3) {
        Ilwis::Point2D<CsyType>& pmin = this->min_corner();
        Ilwis::Point2D<CsyType>& pmax = this->max_corner();
        pmin += {*(il.begin()), *(il.begin() + 1),*(il.begin() + 2)};
        pmax += {*(il.begin()), *(il.begin() + 1),*(il.begin() + 2)};
        normalize();
    }

    return *this;
}

Box3D<CsyType>& operator *=(std::initializer_list<CsyType> il) {
    int size = il.size();
    if ( size == 3) {
        Ilwis::Point2D<CsyType>& pmin = this->min_corner();
        Ilwis::Point2D<CsyType>& pmax = this->max_corner();
        pmin *= {*(il.begin()), *(il.begin() + 1),*(il.begin() + 2)};
        pmax *= {*(il.begin()), *(il.begin() + 1),*(il.begin() + 2)};
        normalize();
    }

   return *this;
}

bool operator==(const Box3D<CsyType>& box ) const {
    return box.max_corner() == this->max_corner() && this->min_corner() == box.min_corner();
}

bool operator!=(const Box3D<CsyType>& box ) const {
    return !(operator==(box));
}

QString toString() const {
    if (std::is_floating_point<CsyType>::value)
        return QString("POLYGON(%1 %2 %3,%4 %5 %6)").arg(this->min_corner().x(),0,'f').
                arg(this->min_corner().y(),0,'f').
                arg(this->min_corner().z(),0,'f').
                arg(this->max_corner().x(),0,'f').
                arg(this->max_corner().y(),0,'f').
                arg(this->max_corner().z(),0,'f');
    else
        return QString("POLYGON(%1 %2 %3,%4 %5 %6)").arg(this->min_corner().x()).
                arg(this->min_corner().y()).
                arg(this->min_corner().z()).
                arg(this->max_corner().x()).
                arg(this->max_corner().y()).
                arg(this->max_corner().z());

}

private:
void normalize() {
    Ilwis::Point3D<CsyType>& pmin = this->min_corner();
    Ilwis::Point3D<CsyType>& pmax = this->max_corner();
    if ( pmin.x() > pmax.x()) {
        CsyType v1 = pmin.x();
        CsyType v2 = pmax.x();
        std::swap(v1, v2);
        pmin.x(v1);
        pmax.x(v2);

    }
    if ( pmin.y() > pmax.y()) {
        CsyType v1 = pmin.y();
        CsyType v2 = pmax.y();
        std::swap(v1, v2);
        pmin.x(v1);
        pmax.x(v2);
    }
    if ( pmin.z() > pmax.z()) {
        CsyType v1 = pmin.z();
        CsyType v2 = pmax.z();
        std::swap(v1, v2);
        pmin.z(v1);
        pmax.z(v2);
    }

}


};


}

typedef Ilwis::Box2D<double> Box2Dd;
typedef Ilwis::Box2D<qint32> Box2Di;
typedef Ilwis::Box2D<quint32> Box2Dui;

BOOST_GEOMETRY_REGISTER_BOX(Box2Dd,Coordinate2d, min_corner(), max_corner())
BOOST_GEOMETRY_REGISTER_BOX(Box2Di,Pixel, min_corner(), max_corner())
BOOST_GEOMETRY_REGISTER_BOX(Box2Dui,Pixel_u, min_corner(), max_corner())


//Q_DECLARE_METATYPE(Ilwis::Box2D<qint8>)
//Q_DECLARE_METATYPE(Ilwis::Box2D<quint8>)
//Q_DECLARE_METATYPE(Ilwis::Box2D<qint16>)
//Q_DECLARE_METATYPE(Ilwis::Box2D<quint16>)
Q_DECLARE_METATYPE(Ilwis::Box2D<qint32>)
Q_DECLARE_METATYPE(Ilwis::Box2D<quint32>)
//Q_DECLARE_METATYPE(Ilwis::Box2D<float>)
Q_DECLARE_METATYPE(Ilwis::Box2D<double>)


#endif // BOX_H
