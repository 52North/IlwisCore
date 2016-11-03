#include <QStringList>
#include <QVector3D>
#include "kernel_global.h"
#include "ilwis.h"
#include "geos/geom/Coordinate.h"
#ifdef Q_OS_WIN
#include "geos/geom/Coordinate.h"
#endif
#include "ilwiscoordinate.h"
#include <cmath>

using namespace Ilwis;

const double crdDeltaLim = 1e-8;

Ilwis::Coordinate::Coordinate() : geos::geom::Coordinate(rUNDEF, rUNDEF, rUNDEF)
{
}

Ilwis::Coordinate::Coordinate(const geos::geom::Coordinate& crd) : geos::geom::Coordinate(crd){
    if (std::isnan(this->z))
        this->z = rUNDEF;
}

Ilwis::Coordinate::Coordinate(const Ilwis::Coordinate& crd) : geos::geom::Coordinate(crd.x,crd.y,crd.z){

}

Ilwis::Coordinate::Coordinate(double px, double py, double pz) : geos::geom::Coordinate(px,py,pz){
}

Ilwis::Coordinate::Coordinate(const std::vector<double>& v)  {
    if ( v.size() < 2) {
        *this = Ilwis::Coordinate();
        return;
    }
    this->x =v[0];
    this->y =v[1];
    if ( v.size() >= 3)
        this->z = v[2];

}

Coordinate::Coordinate(const QVector3D &crd)
{
    x = crd.x();
    y = crd.y();
    z = crd.z();
}

Ilwis::Coordinate::~Coordinate() {
}

/*!
 a point is valid if both its x and y have a valid value;
 * \return
 */
bool Ilwis::Coordinate::isValid() const{
    return x != undefined() && y != undefined() ;
}

bool Ilwis::Coordinate::is0() const{
    return x == 0 && y == 0 ;
}

bool Ilwis::Coordinate::is3D() const {
    if (!isValid())
        return false;
    return z != undefined();
}

Ilwis::Coordinate::operator std::vector<double> () {

    std::vector<double> v;
    if ( !isValid())
        return v;

    if ( is3D()) {
         v = {this->x, this->y, this->z};
        return v;
    }
    v = {this->x, this->y};
    return v;
}

Ilwis::Coordinate& Ilwis::Coordinate::operator=(const Ilwis::Coordinate& p2) {
    this->x = p2.x;
    this->y = p2.y;
    this->z = p2.z;
    return *this;
}

Ilwis::Coordinate& Ilwis::Coordinate::operator=(const Coordinate&& p2) {
    this->x = p2.x;
    this->y = p2.y;
    this->z = p2.z;
    return *this;
}

/*!
 operator += addes a vector of 2 values to a point shifting it in 2D space. Using undefined() values in the vector may lead to unpredictable results
 * \param a vector with a pair of values
 * \return a reference to a shifted 2D point
 */
Coordinate& Ilwis::Coordinate::operator+= (const std::vector<double> &vec){
    if (!this->isValid() || vec.size() < 2){
        *this = Ilwis::Coordinate();
        return *this;
    }

    this->x =this->x + vec[0];
    this->y =this->y + vec[1];
    if ( vec.size() >= 3 && z != undefined())
        this->z =this->z + vec[2];

    return *this;
}

Coordinate& Ilwis::Coordinate::operator-= (const std::vector<double>& vec){
    if (!this->isValid() || vec.size() < 2){
        *this = Ilwis::Coordinate();
        return *this;
    }
    this->x =this->x - vec[0];
    this->y =this->y - vec[1];
    if ( vec.size() >= 3 && z != undefined())
        this->z =this->z - vec[2];

    return *this;
}

double Ilwis::Coordinate::distance(const Coordinate& crd) {
    if ( !crd.isValid() || !this->isValid())
        return this->undefined();
    if ( z == undefined() || crd.z == undefined())
        return std::sqrt(std::pow(abs(this->x - crd.x),2) + std::pow(abs(this->y - crd.y),2));
    return std::sqrt(std::pow(abs(this->x - crd.x),2) + std::pow(abs(this->y - crd.y),2) + std::pow(abs(this->z - crd.z),2));
}

/*!
 operator *= multiplies a point with a factor contained in the vector, shifting it in 2D space. Using undefined() values in the vector may lead to unpredictable results
 * \param  a vector with a pair of values
 * \return a reference to a shifted 2D point
 */
Ilwis::Coordinate& Ilwis::Coordinate::operator*=(const std::vector<double>& vec){
    if (!this->isValid() || vec.size() < 2){
        *this = Ilwis::Coordinate();
        return *this;
    }
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
Ilwis::Coordinate& Ilwis::Coordinate::operator*=(double v){
    if (!this->isValid()){
        *this = Ilwis::Coordinate();
        return *this;
    }
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
Ilwis::Coordinate& Ilwis::Coordinate::operator/=(double v){
    if (!this->isValid() || v == 0){
        *this = Ilwis::Coordinate();
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
 * \return true if the points are at the same Coordinate.
 */
bool Ilwis::Coordinate::operator==(const Ilwis::Coordinate& pnt) const {
    if ( !pnt.isValid() && !this->isValid())
        return true;

    if (!this->isValid() || !pnt.isValid())
        return false;

    return (std::abs(pnt.x -  this->x) < crdDeltaLim) && (std::abs(pnt.y - this->y) < crdDeltaLim) && (std::abs(pnt.z - this->z)<crdDeltaLim);
}

bool Coordinate::operator ==(const geos::geom::Coordinate &pnt) const{
    if (!this->isValid() && (pnt.x == rUNDEF || pnt.y == rUNDEF || std::isnan(pnt.x) || std::isnan(pnt.y)))
        return true;

    return (std::abs(pnt.x -  this->x) < crdDeltaLim) && (std::abs(pnt.y - this->y) < crdDeltaLim) && ((std::abs(pnt.z - this->z)<crdDeltaLim) || (std::isnan(pnt.z) && this->z == rUNDEF));
}
/*!
 compares the coordinates of 2 points to see if they are not equal. comparision with non valid points always leads to true
 * \param pnt, point to be compared
 * \return true if the points are not at the same Coordinate.
 */
bool Ilwis::Coordinate::operator!=(const Ilwis::Coordinate& pnt){
    return !(operator==(pnt));
}

QString Coordinate::toString(int decimals, bool use3D) const
{
    if ( !isValid())
        return "";

    if( use3D)   {
        return QString("%1 %2").arg(x,0,'f', decimals).arg(y,0,'f', decimals).arg(z,0,'f', decimals);
    }
    return QString("%1 %2").arg(x,0,'f', decimals).arg(y,0,'f', decimals);
}

Coordinate& Coordinate::operator =(const geos::geom::Coordinate& crd){
    this->x = crd.x;
    this->y = crd.y;
    if (std::isnan(crd.z))
        this->z = rUNDEF;
    else
        this->z = crd.z;
    return *this;
}

Coordinate &Coordinate::operator =(const QVector3D &p2)
{
    this->x = p2.x();
    this->y = p2.y();
    this->z = p2.z();

    return *this;
}

//-----------------------------------------------
std::vector<double> Ilwis::operator-(const Ilwis::Coordinate& p1, const Ilwis::Coordinate& p2) {
    if (!(p1.isValid() && p2.isValid()))
        return std::vector<double>();
    std::vector<double> v(3,0);
    v[0] = p1.x - p2.x;
    v[1] = p1.y - p2.y ;
    if ( p1.z != p1.undefined() && p2.z != p2.undefined())
        v[2] = p1.z - p2.z ;
    return v;
}

Ilwis::Coordinate Ilwis::operator+(const Ilwis::Coordinate& p1, const std::vector<double>& vec) {
    if (p1.isValid() == false ||  vec.size() < 2 )
        return Ilwis::Coordinate();
    Ilwis::Coordinate p3 = p1;
    p3.x =p1.x + vec[0];
    p3.y = p1.y + vec[1] ;
    if ( vec.size() >= 3 && p1.z != p1.undefined())
        p3.z = p1.z + vec[2];

    return p3;
}

Ilwis::Coordinate Ilwis::operator-(const Ilwis::Coordinate& p1, const std::vector<double>& vec) {
    if (p1.isValid() == false ||  vec.size() < 2 )
        return Ilwis::Coordinate();
    Ilwis::Coordinate p3 = p1;
    p3.x =p1.x - vec[0];
    p3.y = p1.y - vec[1] ;

    if ( vec.size() >= 3 && p1.z != p1.undefined())
        p3.z = p1.z - vec[2];
    return p3;
}

Ilwis::Coordinate Ilwis::operator*(const Ilwis::Coordinate& p1, double v) {
    if (p1.isValid() == false || v == rUNDEF )
        return Ilwis::Coordinate();
    Ilwis::Coordinate p3 = p1;
    p3.x =p1.x * v;
    p3.y = p1.y * v ;
    if ( p1.z != p1.undefined())
        p3.z = p3.z * v;
    return p3;
}

Ilwis::Coordinate Ilwis::operator/(const Ilwis::Coordinate& p1, double v) {
    if (!p1.isValid() || v == 0)
        return Ilwis::Coordinate();
    Ilwis::Coordinate p3 = p1;
    p3.x =p1.x / v;
    p3.y = p1.y / v ;
    if ( p1.z != p1.undefined())
        p3.z = p3.z / v;
    return p3;
}

//---------------------------------------------------------------------------
LatLon::LatLon(){

}

LatLon::LatLon(const Angle &latd, const Angle &lond, double h){
    lat(latd);
    lon(lond);
    z= h;
}

LatLon::LatLon(const QString &lat, const QString &lon)
{
    auto func = [] ( QString value) -> double {
        bool ok;
        double v =  value.toDouble(&ok);
        if ( ok) // we were already a decimal variant; no further parsing needed
            return v;
        value = value.trimmed();
        double sign = 1.0;
        if ( value.right(1) == "S"){
            sign = -1.0;
        }else if ( value.right(1) == "W") {
            sign = -1.0;
        }
        value.right(1).toLong(&ok);
        if ( !ok)
            value = value.left(value.size() -2);
        QStringList parts = value.split(" ");
        v  = parts[0].toDouble(&ok);
        if ( ok && parts.size() > 1) {
            v += parts[1].toDouble(&ok) / 60.0;
            if ( ok && parts.size() == 3){
                v += parts[2].toDouble(&ok)/ 3600.0;
            }
        }
        if ( ok)
            return sign * v;
        return rUNDEF;
    };
    y = func( lat);
    x = func( lon);
    if ( x == rUNDEF || y == rUNDEF)
        x = y = rUNDEF;
}

Angle LatLon::lat() const{
    return y;
}
Angle LatLon::lon() const{
    return x;
}

void LatLon::lat(const Angle& val){
    y = val.degrees();
}

void LatLon::lon(const Angle& val){
    x = val.degrees();
}

QString LatLon::toString(int decimals, bool ) const
{
    if ( !isValid())
        return "";

    auto toDMS = [&](double rDegree) -> QString {
        const QChar chardgr(0260);

        double rDeg = std::abs(rDegree);
        int iDeg = (int)floor(rDeg);
        rDeg -= iDeg;
        rDeg *= 60;
        int iMin = (int)floor(rDeg);
        rDeg -= iMin;
        rDeg *= 60;
        if (rDeg > 59.99) {
          iMin += 1;
          rDeg = 0;
        }
        if (iMin > 59) {
          iDeg += 1;
          iMin -= 60;
        }

        QString result = QString("%1%2%3\'%4\'\'").arg(iDeg, 3, 10).arg(chardgr).arg(iMin, 2, 10, QLatin1Char('0')).arg(rDeg, 3 + decimals, 'f', decimals, QLatin1Char('0'));
        return result;
    };
    QString llstring = QString("%1%2,%3%4").arg(toDMS(y)).arg(y>=0?'N':'S').arg(toDMS(x)).arg(x>=0?'E':'W');

    return llstring;
}
