#ifndef Point_H
#define Point_H

#include <QSize>

#include "ilwis.h"
#include <cstddef>
#include <boost/mpl/int.hpp>
#include <boost\geometry\core\tag.hpp>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/io/wkt/wkt.hpp>
#include <boost/mpl/int.hpp>


namespace bg = boost::geometry;

namespace Ilwis {

class SerializationOptions;

/*!
 * A representation of a point in the cartesian space. It is the base class for the Point2D and Point3D classes. The type
 * of the coordinate (CrdType) must support the ==,+, -, * ,/ and copy operations (usuallu numerical) for this class to work properly.
 * each point has a x and a y property. The meaning of the point is determined by the coordinate space it is used in. On purpose this meaning
 * is now left upto the using context ( realworld coordinate systems, grid system, uni-sphere systems etc...).
 */
template<class CrdType=qint32, int dim=2>
class Point : public bg::model::point<CrdType, dim, bg::cartesian_tag>
{
public:

    /*!
     return the x value of a point
     * \return the x value
     */
    CrdType x() const {
        return this->template get<0>();
    }
    /*!
     return the y value of a point
     * \return the y value
     */
    CrdType y() const {
        return this->template get<1>();
    }

    /*!
     sets the x value of a point
    */
    void x(CrdType v) {
        this->template set<0>(v);
    }
    /*!
     sets the y value of a point
    */
    void y(CrdType v) {
        this->template set<1>(v);
    }
    /*!
     a point is valid if both its x and y have a valid value;
     * \return
     */
    bool isValid() const{
        return x() != undefined && y() != undefined ;
    }

    virtual void store(QDataStream &s, const SerializationOptions &opt) const = 0;

protected:
    Point(){
        init();
    }
    Point(CrdType d1, CrdType d2) {
        init();
        this->template set<0>(d1);
        this->template set<1>(d2);
    }

    CrdType undefined;

private:
    void init() {
        undefined  = undef<CrdType>();
        this->template set<0>(undefined);
        this->template set<1>(undefined);
    }
};

/*!
 * A point in 2D space. It is a derivative of the Point class. As the base class already offer the x,y components no
 *further implementation of these is needed. The class mostely implements special operations on 2D points
 */
template<typename CrdType=qint32> class Point2D : public Point<CrdType> {
public:
    /*!
     default constructor
     */
    Point2D() : Point<CrdType, 2>(){
    }
    /*!
     constructs a 2D point from 2 values. The values must not be undefined else the point remains not valid
     * \param d1 first value
     * \param d2 second value
     */
    Point2D(CrdType d1, CrdType d2) : Point<CrdType, 2>(d1,d2) {
    }

    template<typename U> Point2D(const Point2D<U>& p) {
        if ( p.isValid()) {
            this->x(p.x());
            this->y(p.y());
        } else{
            this->x(this->undefined);
            this->y(this->undefined);

        }
    }

    Point2D(Point2D<CrdType>&& crd) : Point<CrdType, 2>(crd.x(),crd.y()) {
    }

    Point2D(const Point2D<CrdType>& crd) : Point<CrdType, 2>(crd.x(),crd.y()) {
    }

    virtual ~Point2D() {
    }

    Ilwis::Point2D<CrdType>& operator=(const Ilwis::Point2D<CrdType>& p2) {
        this->x( p2.x());
        this->y( p2.y());
        return *this;
    }



    /*!
     Constructs a 2D point from a 3D point. This constructor mainly exist for easy conversion between the two point models. Of course the
     third dimension will be lost after this conversion
     * \param p 3D point
     */
    Point2D(const Point<CrdType,3>& p) : Point<CrdType, 2>(p.x(),p.y()) {
    }
    /*!
     operator += addes a vector of 2 values to a point shifting it in 2D space. Using undefined values in the vector may lead to unpredictable results
     * \param a vector with a pair of values
     * \return a reference to a shifted 2D point
     */
    Point2D<CrdType>& operator+= (const std::vector<CrdType>& vec){
        if (!this->isValid() || !vec.size() != 2)
            return *this;
        this->x(this->x() + vec[0]);
        this->y(this->y() + vec[1]);

        return *this;
    }

    /*!
     operator *= multiplies a point with a factor contained in the vector, shifting it in 2D space. Using undefined values in the vector may lead to unpredictable results
     * \param  a vector with a pair of values
     * \return a reference to a shifted 2D point
     */
    Point2D<CrdType>& operator*=(const std::vector<CrdType>& vec){
        if (!this->isValid() || vec.size() != 2)
            return *this;
        this->x(this->x() * vec[0]);
        this->y(this->y() * vec[1]);

        return *this;
    }

    /*!
     operator *= multipliesthe x,y values with a single factor , shifting it in 2D space. Using undefined values may lead to unpredictable results
     * \param  multiplier
     * \return a reference to a shifted 2D point
     */
    Point2D<CrdType>& operator*=(CrdType v){
        if (!this->isValid())
            return *this;
        this->x(this->x() * v);
        this->y(this->y() * v);

        return *this;
    }

    /*!
     operator /= divides the coordinates of a point with a fixed value. The value must not be zero. If it is zero nothing happens
     Using undefined values may lead to unpredictable results
     * \param v
     * \return
     */
    Point2D<CrdType>& operator/=(CrdType v){
        if (!this->isValid() && v != 0)
            return *this;
        this->x(this->x() / v);
        this->y(this->y() / v);

        return *this;
    }

    /*!
     compares the coordinates of 2 points to see if they are equal. comparision with non valid points always leads to false
     * \param pnt, point to be compared
     * \return true if the points are at the same location.
     */
    bool operator==(const Point2D<CrdType>& pnt) const {
        if (!this->isValid() || !pnt.isValid())
            return false;

        return pnt.x() == this->x() && pnt.y() == this->y();
    }
    /*!
     compares the coordinates of 2 points to see if they are not equal. comparision with non valid points always leads to true
     * \param pnt, point to be compared
     * \return true if the points are not at the same location.
     */
    bool operator!=(const Point2D<CrdType>& pnt){
        return !(operator==(pnt));
    }

    /*!
    \se Ilwis::IlwisObject::store
     */
    void store(QDataStream &s, const SerializationOptions &) const{
        s << QString("2D");
        s << this->x();
        s << this->y();

    }
    /*!
    \se Ilwis::IlwisObject::load
     */
    void load(QDataStream &s){
    }


};

/*!
 * A point in 3D space. It is a derivative of the Point class. As the base class already offer the x,y components no
 *further implementation of these is needed. The class mostely implements special operations on 3D points and a z coordinate
 */
template<typename CrdType=qint32> class Point3D : public Point<CrdType, 3> {
public:
    Point3D() : Point<CrdType, 3>(){

    }
    /*!
     Basic constructor from three values. The last value maybe left undefined in which case it defaults to zero. This is done to make mixing
     two dimensional and three dimensional points easier
     * \param v1 x dimension
     * \param v2 y dimension
     * \param v3 z dimension
     */
    Point3D(CrdType v1, CrdType v2, CrdType v3=0) : Point<CrdType, 3>(v1,v2) {
        z(v3);
    }

    virtual ~Point3D() {

    }

    /*!
     Copy constructor for 2D points. It ensure that 2D and 3D points can be interchanged
     * \param 2D point
     */
    Point3D(const Point2D<CrdType>& p) : Point<CrdType, 3>(p.x(), p.y()) {
        z(0);
    }

    Point3D(Point3D<CrdType>&& crd) : Point<CrdType, 3>(crd.x(),crd.y()) {
        this->z(crd.y());
    }

    Point3D(const Point3D<CrdType>& crd) : Point<CrdType, 3>(crd.x(),crd.y()) {
        this->z(crd.y());
    }

    template<typename U> Point3D(const Point3D<U>& p) {
        this->x(p.x());
        this->y(p.y());
        this->z(p.z());
    }

    template<typename U> Point3D(const Point2D<U>& p) {
        if ( p.isValid()) {
            this->x(p.x());
            this->y(p.y());
            this->z(0);
        } else {
            this->x(this->undefined);
            this->y(this->undefined);
            this->y(this->undefined);

        }
    }

    /*!
     returns the z value of a point
     * \return z value
     */
    CrdType z() const {
        return this->template get<2>();
    }

    /*!
     sets the z value of a point
     * \param z value
     */
    void z(CrdType v) {
        this->template set<2>(v);
    }

    /*!
     Assigns a the values of a 2D point to a 3D point. The z is set to 0
     * \param p2
     * \return
     */
    Ilwis::Point3D<CrdType>& operator=(const Ilwis::Point2D<CrdType>& p2) {
        this->x( p2.x());
        this->y( p2.y());
        this->z(0);
        return *this;
    }

    Ilwis::Point3D<CrdType>& operator=(const Ilwis::Point3D<CrdType>& p2) {
        this->x( p2.x());
        this->y( p2.y());
        this->z(p2.z());
        return *this;
    }

    /*!
     Adds the value of a 3 element vector to the values of coordinate elements. Note that if one the vector values is undefined the behavior of the point becomes undefined.
     If the vector has not three elements nothing will happen.
     * \param a vector with exactly three values.
     * \return the changed point.
     */
    Point3D<CrdType>& operator+= (const std::vector<CrdType>& vec){
        if (!this->isValid() || !vec.size() == 3)
            return *this;
        this->x(this->x() + vec[0]);
        this->y(this->y() + vec[1]);
        this->z(this->z() + vec[2]);

        return *this;
    }

    Point3D<CrdType>& operator+= (CrdType v){
        if (!this->isValid())
            return *this;
        this->x(this->x() + v);
        this->y(this->y() + v);
        this->z(this->z() + v);

        return *this;
    }
    /*!
     Multiplies the value of a 3 element vector with the values of coordinate elements. Note that if one the vector values is undefined the behavior of the point becomes undefined.
     If the vector has not three elements nothing will happen.
     * \param a vector with exactly three values.
     * \return the changed point.
     */
    Point3D<CrdType>& operator*=(const std::vector<CrdType>& vec){
        if (!this->isValid() || !vec.size() == 3)
            return *this;
        this->x(this->x() * vec[0]);
        this->y(this->y() * vec[1]);
        this->z(this->z() * vec[2]);

        return *this;
    }

    /*!
     Multiplies the value with the values of coordinate elements. Note that if one the value is undefined the behavior of the point becomes undefined.
     * \param a value for which the multiplication has been defined
     * \return the changed point.
     */
    Point3D<CrdType>& operator*=(CrdType v){
        if (!this->isValid())
            return *this;
        this->x(this->x() * v);
        this->y(this->y() * v);
        this->z(this->z() * v);

        return *this;
    }
    /*!
     Divides the value with the values of coordinate elements. Note that if one the value is undefined the behavior of the point becomes undefined.
     if the division factor is 0 nothing will happen
     * \param a value for which the division has been defined
     * \return the changed point.
     */
    Point3D<CrdType>& operator/=(CrdType v){
        if (!this->isValid() && v != 0)
            return *this;
        this->x(this->x() / v);
        this->y(this->y() / v);
        this->z(this->z() / v);

        return *this;
    }

    /*!
    \se Ilwis::IlwisObject::store
     */
    void store(QDataStream &s, const SerializationOptions &) const{
        s << QString("3D");
        s << this->x();
        s << this->y();
        s << this->z();

    }
    void load(QDataStream &s){
    }

};

/*!
A coordinate class for use withing geographic coordinate systems. It contains angles for its "x" and "y" members. It does contains a z-value but this is not
often used. The class doesnt map exactly to a Point3D as the unit of x,y and z is different(angles vs meters).
 */
class LatLon : public Ilwis::Point2D<Degrees>{
public:
    LatLon() : Ilwis::Point2D<Degrees>(), _height(Ilwis::rUNDEF) {}
    LatLon(const Degrees& lat, const Degrees& lon, double h=0) : Ilwis::Point2D<Degrees>(lon, lat) , _height(h) {}
    LatLon(const Point3D<double>& crd) : Ilwis::Point2D<Degrees>() {
        x(crd.x());
        y(crd.y());
        z(crd.z());
    }

    LatLon(const LatLon&& ll) :  Point2D<Degrees>() {
       y(ll.y());
       x(ll.x());
       z(ll.z());
    }

    LatLon(const LatLon& ll) :  Point2D<Degrees>() {
       y(ll.y());
       x(ll.x());
       z(ll.z());
    }

    /*!
     returns the z value of a point
     * \return z value
     */
    double z() const {
        return _height;
    }
    /*!
    sets the z-value of a LatLon
     * \param height
     */
    void z(double h){
        _height = h;
    }

    /*!
     returns the north-south position as an angle of a point ("y" direction). The value ranges between -90 and 90
     * \return angle
     */
    double lat(Angle::Unit u=Angle::uDEGREES) const {
        if ( u == Angle::uRADIANS)
            return y().radians();
        else
            return y().degrees();
    }

    /*!
     returns the east-west position as an angle of a point ("x" direction). The value ranges between -180 and 180
     * \return angle
     */
    double lon(Angle::Unit u=Angle::uDEGREES) const {
        if ( u == Angle::uRADIANS)
            return x().radians();
        else
            return x().degrees();
    }

    /*!
     sets the latitude of a LatLon point.
     * \param l
     */
    void setLat(double l) {
        y(l);
    }

    void setLon(double l) {
        x(l);
    }


private:
    double _height;
};



template<typename CrdType=qint32> std::vector<CrdType>
operator-(const Ilwis::Point2D<CrdType>& p1, const Ilwis::Point2D<CrdType>& p2) {
    if (!(p1.isValid() && p2.isValid()))
        return std::vector<CrdType>();
    std::vector<CrdType> v(2);
    v[0] = p1.x() - p2.x();
    v[1] = p1.y() - p2.y() ;
    return v;
}

template<typename CrdType=qint32> Ilwis::Point2D<CrdType>
operator+(const Ilwis::Point2D<CrdType>& p1, const std::vector<CrdType>& vec) {
    if (p1.isValid() == false ||  vec.size() != 2 )
        return Ilwis::Point2D<CrdType>();
    Ilwis::Point2D<CrdType> p3(p1.x() + vec[0], p1.y() + vec[1]) ;
    return p3;
}



template<typename CrdType=qint32> Ilwis::Point2D<CrdType>
operator*(const Ilwis::Point2D<CrdType>& p1, double v) {
    Ilwis::Point2D<CrdType> p3(p1.x() * v, p1.y() * v) ;
    return p3;
}

template<typename CrdType=qint32> Ilwis::Point2D<CrdType>
operator/(Ilwis::Point2D<CrdType>& p1, double v) {
    if (!p1.isValid() || v == 0)
        return Ilwis::Point2D<CrdType>();
    Ilwis::Point2D<CrdType> p3(p1.x() / v, p1.y() / v) ;
    return p3;
}

template<typename CrdType=qint32> Ilwis::Point3D<CrdType>
operator+(const Ilwis::Point3D<CrdType>& p1, const std::vector<CrdType>& vec) {
    if (p1.isValid() == false ||  vec.size() != 2 )
        return Ilwis::Point2D<CrdType>();
    Ilwis::Point3D<CrdType> p3(p1.x() + vec[0], p1.y() + vec[1], p1.z() + vec[2]) ;
    return p3;
}

template<typename CrdType=qint32> std::vector<CrdType>
operator-(const Ilwis::Point3D<CrdType>& p1, const Ilwis::Point3D<CrdType>& p2) {
    if (!(p1.isValid() && p2.isValid()))
        return std::vector<CrdType>();
    std::vector<CrdType> v(3);
    v[0] = p1.x() - p2.x();
    v[1] = p1.y() - p2.y();
    v[2] = p1.z() - p2.z();
    return v;
}

template<typename CrdType=qint32> Ilwis::Point3D<CrdType>
operator*(const Ilwis::Point3D<CrdType>& p1, double v) {
    Ilwis::Point3D<CrdType> p3(p1.x() * v, p1.y() * v, p1.z() * v) ;
    return p3;
}

template<typename CrdType=qint32> bool
operator==(const Ilwis::Point3D<CrdType>& p1, const Ilwis::Point3D<CrdType>& p2) {
    return p1.x() == p2.x() && p1.y() == p2.y() && p1.z() == p2.z();
}

template<typename CrdType=qint32> bool
operator!=(const Ilwis::Point3D<CrdType>& p1, const Ilwis::Point3D<CrdType>& p2) {
    return ! operator==(p1,p2);
}

template<typename CrdType=qint32> Ilwis::Point3D<CrdType>
operator/(Ilwis::Point3D<CrdType>& p1, double v) {
    if (!p1.isValid() || v == 0)
        return Ilwis::Point3D<CrdType>();
    Ilwis::Point3D<CrdType> p3(p1.x() / v, p1.y() / v, p1.z() / v) ;
    return p3;
}

//template<class PointType > QDataStream & operator << ( QDataStream & s, const PointType& pnt ){

//    pnt.store(s, SerializationOptions());

//    return s;
//}
} // end Ilwis namespace

typedef Ilwis::Point2D<> Pixel;
typedef Ilwis::Point3D<> Voxel;
typedef Ilwis::Point3D<double> Coordinate;
typedef Ilwis::Point2D<double> Coordinate2d;
typedef Ilwis::Point2D<quint32>  Pixel_u;
typedef Ilwis::Point3D<quint32>  Voxel_u;

BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(Coordinate2d, double, cs::cartesian, x, y, x, y)
BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(Pixel, qint32, cs::cartesian, x, y, x, y)
BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(Pixel_u, quint32, cs::cartesian, x, y, x, y)
BOOST_GEOMETRY_REGISTER_POINT_3D_GET_SET(Coordinate, double, cs::cartesian, x, y, z, x, y, z)
BOOST_GEOMETRY_REGISTER_POINT_3D_GET_SET(Voxel, qint32, cs::cartesian, x, y, z, x, y, z)
BOOST_GEOMETRY_REGISTER_POINT_3D_GET_SET(Voxel_u, quint32, cs::cartesian, x, y, z, x, y, z)








#endif // Point_H
