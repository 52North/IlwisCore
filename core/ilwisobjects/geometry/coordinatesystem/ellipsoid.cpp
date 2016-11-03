#include <QString>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>

#include "kernel.h"
#include "ilwisangle.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "ilwiscontext.h"
#include "resource.h"
#include "ellipsoid.h"

using namespace Ilwis;

const IEllipsoid Ellipsoid::wgs84ellipsoid; //=IEllipsoid(QUrl(WGS84RESOURCE),QExplicitlySharedDataPointer<Ellipsoid>(new Ilwis::Ellipsoid(6378137.0,298.257223563)));
std::map<QString, QString> Ellipsoid::_ellcode2Name;


Ellipsoid::Ellipsoid() : IlwisObject()
{
    setEllipsoid(6371007.1809185,0);
}

Ellipsoid::Ellipsoid(const Resource& resource) : IlwisObject(resource)
{
}

Ellipsoid::Ellipsoid(double a, double invf) : IlwisObject()
{
    setEllipsoid(a, invf);
}

Ellipsoid::~Ellipsoid() {
}

bool Ellipsoid::isSpherical() const {
     return _flattening == 0;
}

bool Ellipsoid::isEqual(const IEllipsoid& ellips) const{
    double delta1 = abs(ellips->majorAxis() - majorAxis());
    double delta2 = abs(ellips->excentricity() - excentricity());
    return delta1 < 0.1 && delta2 < EPS6;
}

bool Ellipsoid::isValid() const{
    return false;
}

Coordinate Ellipsoid::latlon2Coord(const LatLon& sourceLatLon) const{
    Coordinate result;
    double phi = sourceLatLon.lon().radians();
    double lambda = sourceLatLon.lat().radians();
    double sinPhi = sin(phi);
    double cosPhi = cos(phi);
    double sinLambda = sin(lambda);
    double cosLambda = cos(lambda);
    double e2 = 2 * _flattening - _flattening * _flattening;
    double N = _majorAxis / sqrt(1 - e2 * sinPhi * sinPhi);
    double h = sourceLatLon.z;
    result.x = (N + h) * cosPhi * cosLambda;
    result.y  = (N + h) * cosPhi * sinLambda;
    result.z = (N * (1 - e2) + h) * sinPhi;
    return result;
}

LatLon Ellipsoid::coord2latlon(const Coordinate& crdSource) const{

    if ( crdSource.z == 0)
        return LatLon();
    LatLon result;
    double r = sqrt(crdSource.x * crdSource.x + crdSource.y * crdSource.y);
    double phiTry = 0;
    double e2 = 2 * _flattening - _flattening * _flattening;
    double phiNext = atan2((crdSource.z + e2*crdSource.z),r);
    double N = _majorAxis; // start approximation
    while (abs(phiNext - phiTry) > EPS15 ) {
        phiTry = phiNext; // update trial phi
        N = _majorAxis / sqrt(1 - e2*sin(phiNext)*sin(phiNext));
        phiNext = atan2((crdSource.z + e2*N*sin(phiNext)),r);
    }
    result.x = Angle(phiNext, true);//set phi back to lat in degrees
    result.y = Angle(atan2( crdSource.y, crdSource.x), true);// lon in degrees
    result.z = Angle(r*cos(phiNext) + crdSource.z*sin(phiNext)
            - _majorAxis*sqrt(1 - e2*sin(phiNext)*sin(phiNext)),true);
    return result;
}

LatLon Ellipsoid::latlon2Coord(const IEllipsoid &sourceEllipsoid, const LatLon& sourceLatLon) const{
    if ( sourceLatLon.z == 0)
        return LatLon();

    double mas = sourceEllipsoid->majorAxis();
    double mis = sourceEllipsoid->minorAxis();
    double da = _majorAxis - mas;
    double df = _flattening - sourceEllipsoid->flattening();

    double phi = sourceLatLon.lon().radians();
    double lam = sourceLatLon.lat().radians();
    double h = sourceLatLon.z;
    double sinPhi = sin(phi);
    double cosPhi = cos(phi);
    double sin2Phi = sinPhi * sinPhi;
    double e2 = 2 * sourceEllipsoid->flattening() - sourceEllipsoid->flattening() * sourceEllipsoid->flattening();
    // n = radius of curvature in the prime vertical
    double n = mas / sqrt(1 - e2 * sin2Phi);
    // m = radius of curvature in the meridian
    double rTmp = 1 - e2 * sin2Phi;
    double m = mas * (1 - e2) / sqrt(rTmp * rTmp * rTmp);


    double dPhi = da * (n * e2 * sinPhi * cosPhi) / mas
           + df * (m * mas / mis + n * mis / mas) * sinPhi * cosPhi;
    dPhi /= m + h;

    double dh = - da * mas / n + df * n * sin2Phi * mis / mas;

    phi += dPhi;

    h += dh;
    LatLon result;
    result.x = Angle(phi,true);
    result.y = Angle(lam, true);
    result.z = Angle(h, true);
    return result;
}

/*
    Formules from "Map Projections - A Working Manual"  John P.Snyder USGS Professional Paper 1395
    US Govt Printing Office, Washington 1987  page 199
    following Robbin's inverse of 'Clarke's best formula' (see also Bomford G. 1971, Geodesy:
    Oxford England, Clarendon Press
  suitable for lines up to 800 km;
*/
double Ellipsoid::distance(const LatLon &begin, const LatLon& end) const{
    double c = rUNDEF;
    if (abs(begin.lon()) + EPS15 > 90 || abs(end.lon()) + EPS15 > 90 )
        return c; // invalid latitudes for reliable computation
    double phi1 = begin.lon().radians();
    double lam1 = begin.lat().radians();
    double phi2 = end.lon().radians();
    double lam2 = end.lat().radians();
    double e2 = 2 * _flattening - _flattening * _flattening;

    double N1 = _majorAxis /sqrt(1 - e2 * sin(phi1)*sin(phi1));
    double N = _majorAxis /sqrt(1 - e2 * sin(phi2)*sin(phi2));
    double psi = atan((1-e2)*tan(phi2) + e2*N1*sin(phi1)/N/cos(phi2));
    double azim = atan2(sin(lam2-lam1),cos(phi1)*tan(psi) - sin(phi1)*cos(lam2-lam1));
    long signCosAz = (cos(azim)>0)?1:-1;
    double s;
    if (abs(sin(azim)) < EPS12 )
        s = signCosAz * asin(cos(phi1)*sin(psi) - sin(phi1)*cos(psi));
    else
        s = asin(sin(lam2-lam1)*cos(psi)/sin(azim));
    double G = _excentricity * sin(phi1)/sqrt(1 - e2);
    double H = _excentricity * cos(phi1)*cos(azim)/sqrt(1 - e2);
    double s2 = s*s;
    double s4 =s2*s2;
    c = N1*s*(1.0-s2*H*H*(1.0-H*H)/6+(s*s2/8)*G*H*(1-2*H*H)
              +(s4/120)*(H*H*(4-7*H*H)-3*G*G*(1-7*H*H)) - (s4*s/48)*G*H);
    return c;
}

double Ellipsoid::azimuth(const LatLon& begin, const LatLon& end) const{
    double azim = rUNDEF;
    if (abs(begin.lon()) + EPS15 > 90 || abs(end.lon()) + EPS15 > 90 )
        return azim; // invalid latitudes for reliable computation
    double phi1 = begin.lon().radians();
    double lam1 = begin.lat().radians();
    double phi2 = end.lon().radians();
    double lam2 = end.lat().radians();
    double e2 = 2 * _flattening - _flattening * _flattening;
    double N1 = _majorAxis /sqrt(1 - e2 * sin(phi1)*sin(phi1));
    double N = _majorAxis /sqrt(1 - e2 * sin(phi2)*sin(phi2));
    double psi = atan((1-e2)*tan(phi2) + e2*N1*sin(phi1)/N/cos(phi2));
    azim = atan2(sin(lam2-lam1),cos(phi1)*tan(psi) - sin(phi1)*cos(lam2-lam1)) * 180.0 / M_PI;
    return azim;
}

Coordinate Ellipsoid::coord2coord(const Coordinate& ctsIn, const Coordinate& ctsPivot,
                                                        double tx, double ty, double tz,
                                                        double Rx, double Ry, double Rz,
                                                        double s) const//3D linearized similarity
{
    ///3 + 3 + 3 + 1 transformation constants
//double x0, y0, z0; // position of transf centroid (0,0,0 if centroid == geocenter)
//double tx, ty, tz; // translations (shifts)  meters
//double Rx, Ry, Rz; // rotations (off-diagonal elements in rot matrix, quasi sines) radians
//double s;          //  scale difference (diagonal elements in rot matrix) nano-radians
    Coordinate ctsOut;
    double x = ctsIn.x;
    double y = ctsIn.y;
    double z = ctsIn.z;
    double x0 = ctsPivot.x;
    double y0 = ctsPivot.y;
    double z0 = ctsPivot.z;
    // input coordinates from reference_system_1 (meters geocentric)

    ctsOut.x = x + tx + s * (x - x0) + Rz * (y - y0) - Ry * (z - z0) ;
    ctsOut.y = y + ty - Rz * (x - x0) + s * (y - y0) + Rx * (z - z0) ;
    ctsOut.z = z + tz + Ry * (x - x0) - Rx * (y - y0) + s * (z - z0) ;
  // output coordinates to reference_system_2  (meters geocentric)

    return ctsOut;
}

double Ellipsoid::majorAxis() const{
    return _majorAxis;
}

double Ellipsoid::minorAxis() const{
    return _minoraxis;
}

double Ellipsoid::flattening() const{
    return _flattening;
}

double Ellipsoid::excentricity() const{
    return _excentricity;
}

double Ellipsoid::excentricity2() const
{
    return _excentricity * _excentricity;
}

void Ellipsoid::setEllipsoid( double a, double invf){
    InternalDatabaseConnection db;
    QString query = "Select * from ellipsoid";
    if ( db.exec(query) ){
        while( db.next()){
             QSqlRecord rec = db.record();
             double maxis = rec.field("majoraxis").value().toDouble();
             double invflat = rec.field("invflattening").value().toDouble();
             if ( a == 6378137 && invf == 0){ // special case that is basically an error but is assumed to refered to the wgs84 ellipsoid
                 fromInternal(rec);
                 return;
             }else if ( std::abs(maxis - a) < 0.01 && std::abs(invflat - invf) < 0.0000001){
                 fromInternal(rec);
                 return;
             }
        }
    }
    _flattening = (invf <= 1) ? 0 : 1.0/invf; // invf = 0 for spheres
    _majorAxis = a;
    _minoraxis = _majorAxis * (1.0 - _flattening);
    _excentricity = sqrt( 1.0 - (_minoraxis * _minoraxis) / (_majorAxis * _majorAxis));

    QString newName = (invf == 0) ? "Sphere" : "User Defined";
    name(newName);
    code(newName);
}

void Ellipsoid::fromInternal(const QSqlRecord& rec ){
    IlwisObject::fromInternal(rec);
//    name(rec.field("name").value().toString()); // override the one from IlwisObject::fromInternal()
    setWKTName(rec.field("wkt").value().toString());
    setAuthority(rec.field("authority").value().toString());
    double invf = rec.field("invflattening").value().toDouble();;
    _flattening = (invf <= 1) ? 0 : 1.0/invf; // invf = 0 for spheres
    _majorAxis = rec.field("majoraxis").value().toDouble();
    _minoraxis = _majorAxis * (1.0 - _flattening);
    _excentricity = sqrt( 1.0 - (_minoraxis * _minoraxis) / (_majorAxis * _majorAxis));
}

IlwisObject *Ellipsoid::clone()
{
    Ellipsoid *ell = new Ellipsoid();
    copyTo(ell);
    return ell;
}

void Ellipsoid::copyTo(IlwisObject *e)
{
    IlwisObject::copyTo(e);
    Ellipsoid *ell = static_cast<Ellipsoid *>(e);
    ell->_majorAxis = _majorAxis;
    ell->_minoraxis = _minoraxis;
    ell->_flattening = _flattening;
    ell->_excentricity = _excentricity;
    ell->_authority = _authority;
    ell->_wkt = _wkt;
}

QString Ellipsoid::authority() const
{
    return _authority;
}

void Ellipsoid::setAuthority(const QString& auth){
    _authority = auth;
}

QString Ellipsoid::toProj4() const
{
    return QString("+a=%1 +b=%2").arg(_majorAxis,0,'f').arg(_minoraxis,0,'f');
}

QString Ellipsoid::toWKT(quint32 spaces) const
{
    QString indent = QString(" ").repeated(spaces);
    QString ell = indent + "ELLIPSOID[\"";
    if (_wkt != "") {
        ell += _wkt;
    }else{
        ell += name();
    }
    ell += "\"," + QString::number(_majorAxis) + "," + QString::number(_flattening);
    ell += "]";
    return ell;
}

QString Ellipsoid::wktShort() const
{
    return _wkt;
}

IlwisTypes Ellipsoid::ilwisType() const
{
    return itELLIPSOID;
}

void Ellipsoid::setWKTName(const QString &wkt)
{
    _wkt = wkt;
}

QString Ellipsoid::ellipsoidCode2Name(const QString &code)
{
    if ( _ellcode2Name.size() == 0){
        InternalDatabaseConnection db;
        QString query = "Select code, name from ellipsoid";
        db.exec(query);
        while(db.next()){
            QString code = db.value(0).toString();
            QString nme = db.value(1).toString();
            _ellcode2Name[code] = nme;
        }
    }

    auto iter = _ellcode2Name.find(code);
    if ( iter != _ellcode2Name.end())
         return iter->second;
    return sUNDEF;
}






