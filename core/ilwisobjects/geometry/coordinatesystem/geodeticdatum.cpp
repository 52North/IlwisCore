#include <QSharedPointer>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>

#include "kernel.h"
#include "module.h"
#include "connectorinterface.h"
#include "identity.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"

using namespace Ilwis;

GeodeticDatum::GeodeticDatum(const QString& name) : Identity(name)
{
    _datumParams.resize(10);
    std::fill(_datumParams.begin(),_datumParams.end(), 0.0);
    _datumParams[dmSCALE] = 1;
    _mode = dtMolodensky;
    _isValid = false;

}

GeodeticDatum::GeodeticDatum(std::vector<double> &datumParameters, const IEllipsoid &ellips)
{
    if ( datumParameters.size() == 3) {
        set3TransformationParameters(datumParameters[0], datumParameters[1], datumParameters[2], ellips);
    } else if ( datumParameters.size() == 7) {
        set7TransformationParameters(datumParameters[0], datumParameters[1], datumParameters[2],
                datumParameters[3], datumParameters[4], datumParameters[5], datumParameters[6]);
    } else if ( datumParameters.size() == 10) {
        set10TransformationParameters(datumParameters[0], datumParameters[1], datumParameters[2],
                datumParameters[3], datumParameters[4], datumParameters[5],
                datumParameters[6], Coordinate(datumParameters[7], datumParameters[8], datumParameters[9]));
    }
}

void GeodeticDatum::set3TransformationParameters(const double x, const double y, const double z, const IEllipsoid & ellips) {
    _datumParams[dmDX] = x;
    _datumParams[dmDY] = y;
    _datumParams[dmDZ] = z;
    _mode = dtMolodensky;
    code(QString("+towgs84=%1,%2,%3").arg(x).arg(y).arg(z));
    getFromInternal(ellips->code()); // table datums.csv isn't yet complete; we can only do this for 3 parameter molodensky
    _isValid = true;
}

void GeodeticDatum::set7TransformationParameters(const double x, const double y, const double z, const double rx, const double ry, const double rz, const double scale) {
    _datumParams[dmDX] = x;
    _datumParams[dmDY] = y;
    _datumParams[dmDZ] = z;
    _datumParams[dmRX] = rx;
    _datumParams[dmRY] = ry;
    _datumParams[dmRZ] = rz;
    _datumParams[dmSCALE] = scale;
    _mode = dtBursaWolf;
    code(QString("%1,%2,%3,%4,%5").arg(code()).arg(rx).arg(ry).arg(rz).arg(scale));
    _isValid = true;
}

void GeodeticDatum::set10TransformationParameters(const double x, const double y, const double z, const double rx, const double ry, const double rz, const double scale, const Coordinate & center) {
    _datumParams[dmDX] = x;
    _datumParams[dmDY] = y;
    _datumParams[dmDZ] = z;
    _datumParams[dmRX] = rx;
    _datumParams[dmRY] = ry;
    _datumParams[dmRZ] = rz;
    _datumParams[dmSCALE] = scale;
    _datumParams[dmCENTERXR] = center.x;
    _datumParams[dmCENTERXR] = center.y;
    _datumParams[dmCENTERXR] = center.z;
    _mode = dtBadekas;
    code(QString("%1,%2,%3,%4").arg(code()).arg( center.x).arg( center.y).arg( center.z));
    _isValid = true;
}

void GeodeticDatum::getFromInternal(const QString &ellips) {
    InternalDatabaseConnection db;
    QString query = "Select * from datum";
    if ( db.exec(query) ){
        while( db.next()){
             QSqlRecord rec = db.record();
             double dx = rec.field("dx").value().toDouble();
             double dy = rec.field("dy").value().toDouble();
             double dz = rec.field("dz").value().toDouble();
             QString ellipsoid = rec.field("ellipsoid").value().toString();
             // the combination of ellips-name, dx, dy and dz uniquely identifies the datum by name and area
             if ( ellipsoid.toLower() == ellips.toLower() && std::abs(_datumParams[dmDX] - dx) < 0.01 && std::abs(_datumParams[dmDY] - dy) < 0.01 && std::abs(_datumParams[dmDZ] - dz) < 0.01) {
                 _datumParams[dmDX] = dx;
                 _datumParams[dmDY] = dy;
                 _datumParams[dmDZ] = dz;
                 code(rec.field("code").value().toString());
                 name(rec.field("name").value().toString());
                 setArea(rec.field("area").value().toString());
                 setWktName(rec.field("wkt").value().toString());
                 setAuthority(rec.field("authority").value().toString());
                 return;
             }
        }
    }
}

GeodeticDatum *GeodeticDatum::clone() const
{
    GeodeticDatum *datum = new GeodeticDatum();
    datum->name(name())    ;
    datum->code(code());
    datum->setDescription(description());
    datum->_area = _area;
    datum->_authority = _authority;
    datum->_datumParams = _datumParams;
    datum->_mode = _mode;
    datum->_isValid = _isValid;
    datum->_wkt = _wkt;

    return datum;
}

double GeodeticDatum::parameter(DatumParameters parm) const {
    return _datumParams[parm];
}

bool GeodeticDatum::isValid() const
{
    bool ok = true;
    int count = 0;
    for(auto parm : _datumParams){
        if ( count != dmSCALE)
            ok |= parm != 0;
        ++count;
    }
    return ok && name() != sUNDEF;
}

GeodeticDatum::DatumTransformation GeodeticDatum::getTransformationMode() const {
    return _mode;
}

void GeodeticDatum::fromCode(const QString &gcode)
{
    InternalDatabaseConnection stmt;
    QString query = QString("Select * from datum where code='%1' or code='%2'").arg(gcode).arg('D' + gcode); // temporary workaround (only working for wgs84) because file datums.csv does not have datumnames that match the ones in epsg.pcs

    if (stmt.exec(query)) {
        if ( stmt.next()) {
            QString area = stmt.value(stmt.record().indexOf("area")).toString();
            QString geocode = stmt.value(stmt.record().indexOf("code")).toString();
            QString ellipsoid = stmt.value(stmt.record().indexOf("ellipsoid")).toString();
            double dx = stmt.value(stmt.record().indexOf("dx")).toDouble();
            double dy = stmt.value(stmt.record().indexOf("dy")).toDouble();
            double dz = stmt.value(stmt.record().indexOf("dz")).toDouble();
            setArea(area);
            code(geocode);
            setWktName(stmt.value(stmt.record().indexOf("wkt")).toString());
            IEllipsoid ellips;
            ellips.prepare("code=ellipsoid:" + ellipsoid);
            set3TransformationParameters(dx, dy, dz, ellips);
        } else {
            kernel()->issues()->log(TR("No datum for this code %1").arg(gcode));
        }
    } else {
        kernel()->issues()->logSql(stmt.lastError());
    }
}

void GeodeticDatum::setWktName(const QString &name)
{
    _wkt = name;
}

QString GeodeticDatum::toWKT(quint32 spaces) const
{
    if ( !isValid())
        return "";

    QString dat;
    QString indent = QString(" ").repeated(spaces);
    QString ending = spaces == 0 ? "" : "\n";
    if ( _wkt.size() > 0)
        dat = indent + "DATUM[\"" + _wkt + "," + ending;
    else{
        dat = indent + "DATUM[\" "+ (name() != sUNDEF ? name() : "UNKNOWN") + "\"," + ending;
        dat += indent + "[" + code().toUpper() + "]" + ending;
        dat.replace("+","");
        dat.replace("=","[");
    }

    return dat;
}

QString GeodeticDatum::area() const
{
    return _area;
}

void GeodeticDatum::setArea(const QString &v)
{
    _area = v;
}

QString GeodeticDatum::authority() const
{
    return _authority;
}

void GeodeticDatum::setAuthority(const QString& auth){
    _authority = auth;
}






