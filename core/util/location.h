#ifndef Point_H
#define Point_H

#include <QSize>
#include "ilwisangle.h"

namespace Ilwis {

/*!
 * A point in 2D space. It is a derivative of the Point class. As the base class already offer the x,y components no
 *further implementation of these is needed. The class mostely implements special operations on 2D points
 */
template<class CrdType=qint32, bool useDouble = false> class Location{
public:
    /*!
     default constructor
     */
    Location() : x(undefined()), y(undefined()), z(undefined()){
    }
    /*!
     constructs a 2D point from 2 values. The values must not be undefined() else the point remains not valid
     * \param d1 first value
     * \param d2 second value
     */
    Location(CrdType d1, CrdType d2, CrdType d3=Location::undefined())  : x(d1), y(d2), z(d3){
    }

    template<typename U> Location(const Location<U>& p) {
        if ( p.isValid()) {
            this->x =(CrdType)p.x;
            this->y =(CrdType)p.y;
            this->z =(CrdType)p.z;
        } else{
            this->x =undefined();
            this->y =undefined();
            this->z = undefined();

        }
    }

    Location(const Location<CrdType>& p) {
        if ( p.isValid()) {
            this->x =p.x;
            this->y =p.y;
            this->z =p.z;
        } else{
            this->x =undefined();
            this->y =undefined();
            this->z = undefined();

        }
    }

    Location(Location<CrdType>&& crd) : x(crd.x), y(crd.y), z(crd.z){
        crd.x = crd.y = this->undefined();
    }


    Location(const QVariantMap& l){
        bool ok;
        double x = l["x"].toDouble(&ok);
        if (ok){
            double y = l["y"].toDouble(&ok);
            if (ok){
                double z = l["z"].toDouble(&ok);
                if (ok){
                    this->x = (CrdType)x;
                    this->y = (CrdType)y;
                    this->z = (CrdType)z;
                    return;
                }
            }
        }
    }


    Location(const std::vector<CrdType>& v) : Location() {
        if ( v.size() < 2) {
            *this = Location<CrdType>();
            return;
        }
        this->x =v[0];
        this->y =v[1];
        if ( v.size() >= 3)
            this->z = v[2];
     }

    virtual ~Location() {
    }

    /*!
     a point is valid if both its x and y have a valid value;
     * \return
     */
    bool isValid() const{
        return x != undefined() && y != undefined() ;
    }

    bool is0() const{
        return x == 0 && y == 0 ;
    }

    bool is3D() const {
        return isValid() && (z != undefined());
    }

    operator std::vector<CrdType> () {
        std::vector<CrdType> v {this->x, this->y, this->z};
        return v;
    }

    Ilwis::Location<CrdType>& operator=(const Ilwis::Location<CrdType>& p2) {
        this->x = p2.x;
        this->y = p2.y;
        this->z = p2.z;
        return *this;
    }

    Ilwis::Location<CrdType>& operator=(const Ilwis::Location<CrdType>&& p2) {
        this->x = p2.x;
        this->y = p2.y;
        this->z = p2.z;
        return *this;
    }

    void store(QDataStream& stream){
        stream << this->x;
        stream << this->y;
        stream << this->z;
    }

    void load(QDataStream& stream){
        stream >> this->x;
        stream >> this->y;
        stream >> this->z;
    }

    /*!
     operator += addes a vector of 2 values to a point shifting it in 2D space. Using undefined() values in the vector may lead to unpredictable results
     * \param a vector with a pair of values
     * \return a reference to a shifted 2D point
     */
    Location<CrdType>& operator+= (const std::vector<CrdType>& vec){
        if (!this->isValid() || vec.size() < 2)
            return *this;

        this->x =this->x + vec[0];
        this->y =this->y + vec[1];
        if ( vec.size() >= 3 && z != undefined())
            this->z =this->z + vec[2];

        return *this;
    }

    Location<CrdType>& operator-= (const std::vector<CrdType>& vec){
        if (!this->isValid() || vec.size() < 22)
            return *this;
        this->x =this->x - vec[0];
        this->y =this->y - vec[1];
        if ( vec.size() >= 3 && z != undefined())
            this->z =this->z - vec[2];

        return *this;
    }

    double distance(const Location<CrdType>& crd) {
        if ( !crd.isValid() || !this->isValid())
            return rUNDEF;
        if ( z == undefined() || crd.z == undefined())
            return std::sqrt(std::pow(abs(this->x - crd.x),2) + std::pow(abs(this->y - crd.y),2));
        return std::sqrt(std::pow(abs(this->x - crd.x),2) + std::pow(abs(this->y - crd.y),2) + std::pow(abs(this->z - crd.z),2));
    }

    /*!
     operator *= multiplies a point with a factor contained in the vector, shifting it in 2D space. Using undefined() values in the vector may lead to unpredictable results
     * \param  a vector with a pair of values
     * \return a reference to a shifted 2D point
     */
    Location<CrdType>& operator*=(const std::vector<CrdType>& vec){
        if (!this->isValid() || vec.size() < 2)
            return *this;
        this->x =this->x * vec[0];
        this->y =this->y * vec[1];
        if ( vec.size() >= 3 && z != undefined())
            this->z =this->z * vec[2];

        return *this;
    }

    /*!
     operator *= multipliesthe x,y values with a single factor , shifting it in 2D space. Using undefined() values may lead to unpredictable results
     * \param  multiplier
     * \return a reference to a shifted 2D point
     */
    Location<CrdType>& operator*=(CrdType v){
        if (!this->isValid())
            return *this;
        this->x =this->x * v;
        this->y =this->y * v;
        if ( z != undefined())
            this->z =this->z * v;

        return *this;
    }

    /*!
     operator /= divides the coordinates of a point with a fixed value. The value must not be zero. If it is zero nothing happens
     Using undefined() values may lead to unpredictable results
     * \param v
     * \return
     */
    Location<CrdType>& operator/=(CrdType v){
        if (!this->isValid() || v == 0){
            *this = Location<CrdType>();
            return *this;
        }
        this->x =this->x / v;
        this->y =this->y / v;
        if ( z != undefined())
            this->z =this->z / v;

        return *this;
    }

    /*!
     compares the coordinates of 2 points to see if they are equal. comparision with non valid points always leads to false
     * \param pnt, point to be compared
     * \return true if the points are at the same location.
     */
    bool operator==(const Location<CrdType>& pnt) const {
        if (!this->isValid() && !pnt.isValid())
            return true;

        if (!this->isValid() || !pnt.isValid())
            return false;

        return pnt.x == this->x && pnt.y == this->y && pnt.z == this->z;
    }
    /*!
     compares the coordinates of 2 points to see if they are not equal. comparision with non valid points always leads to true
     * \param pnt, point to be compared
     * \return true if the points are not at the same location.
     */
    bool operator!=(const Location<CrdType>& pnt){
        return !(operator==(pnt));
    }

    operator std::vector<CrdType>() const{
        std::vector<CrdType> v;
        if( is3D())
            v  = {x,y,z};
        else
            v = {x,y};
        return v;


    }

    static double undefined(){ return  useDouble ? rUNDEF : iUNDEF;}
    static quint64 valuetype(){ return useDouble ? itDOUBLE : itINTEGER;}

    CrdType x;
    CrdType y;
    CrdType z;

};


template<typename CrdType>
std::vector<CrdType> operator-(const Ilwis::Location<CrdType>& p1, const Ilwis::Location<CrdType>& p2) {
    if (!(p1.isValid() && p2.isValid()))
        return std::vector<int>();
    std::vector<CrdType> v(3,0);
    v[0] = p1.x - p2.x;
    v[1] = p1.y - p2.y ;
    if ( p1.z != p1.undefined() && p2.z != p2.undefined())
        v[2] = p1.z - p2.z ;
    return v;
}

template<typename CrdType>
Ilwis::Location<CrdType> operator+(const Ilwis::Location<CrdType>& p1, const std::vector<double>& vec) {
    if (p1.isValid() == false ||  vec.size() < 2 )
        return Ilwis::Location<CrdType>();
    Ilwis::Location<CrdType> p3(p1);
    p3.x = p1.x + vec[0];
    p3.y = p1.y + vec[1] ;
    if ( vec.size() >= 3 && p1.z != p1.undefined())
        p3.z  = p1.z + vec[2];

    return p3;
}

template<typename CrdType>
Ilwis::Location<CrdType> operator+(const Ilwis::Location<CrdType>& p1, const Ilwis::Location<CrdType>& p2) {
    if (p1.isValid() == false ||  p2.isValid() == false )
        return Ilwis::Location<CrdType>();
    Ilwis::Location<CrdType> p3;
    p3.x = p1.x + p2.x;
    p3.y = p1.y + p2.y ;
    if ( p2.z != p2.undefined() && p1.z != p1.undefined())
        p3.z  = p1.z + p2.z;

    return p3;
}
template<typename CrdType>
Ilwis::Location<CrdType> operator-(const Ilwis::Location<CrdType>& p1, const std::vector<double>& vec) {
    if (p1.isValid() == false ||  vec.size() < 2 )
        return Ilwis::Location<CrdType>();
    Ilwis::Location<CrdType> p3(p1);
    p3.x = p1.x - vec[0];
    p3.y = p1.y - vec[1] ;

    if ( vec.size() >= 3 && p1.z != p1.undefined())
        p3.z = p1.z - vec[2];
    return p3;
}

template<typename CrdType>
Ilwis::Location<CrdType> operator*(const Ilwis::Location<CrdType>& p1, double v) {
    Ilwis::Location<CrdType> p3(p1.x * v, p1.y * v) ;
    if ( p1.z != p1.undefined())
        p3.z = p1.z * v;
    return p3;
}

template<typename CrdType>
Ilwis::Location<CrdType> operator/(const Ilwis::Location<CrdType>& p1, double v) {
    if (!p1.isValid() || v == 0)
        return Ilwis::Location<CrdType>();
    Ilwis::Location<CrdType> p3(p1.x / v, p1.y / v) ;
    if ( p1.z != p1.undefined())
        p3.z = p1.z / v;
    return p3;
}

typedef Location<int> Pixel;
typedef Location<int> XY; // though technically the same as a pixel, it is meant for locations that are not pixels
typedef Location<double> Pixeld;

} // end Ilwis namespace

Q_DECLARE_METATYPE(Ilwis::Pixel)
Q_DECLARE_METATYPE(Ilwis::Pixeld)












#endif // Point_H
