#include <QSharedPointer>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include "kernel.h"
#include "module.h"
#include "connectorinterface.h"
#include "identity.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "geodeticdatum.h"

using namespace Ilwis;

GeodeticDatum::GeodeticDatum(const QString& name) : Identity(name)
{
    _datumParams.resize(10);
    std::fill(_datumParams.begin(),_datumParams.end(), 0.0);
    _datumParams[dmSCALE] = 1;
    _isValid = false;

}

void GeodeticDatum::set3TransformationParameters(double x, double z, double y){
    _datumParams[dmDX] = x;
    _datumParams[dmDY] = y;
    _datumParams[dmDZ] = z;
    code(QString("+towgs84=%1,%2,%3").arg(x).arg(y).arg(z));
     _isValid = true;
}

void GeodeticDatum::set7TransformationParameters(double x, double z, double y, double rx, double ry, double rz, double scale){
    set3TransformationParameters(x,y,z);
    _datumParams[dmRX] = rx;
    _datumParams[dmRY] = ry;
    _datumParams[dmRZ] = rz;
    _datumParams[dmSCALE] = scale;
   code(QString("%1,%2,%3,%4,%5").arg(code()).arg(rx).arg(ry).arg(rz).arg(scale));


}

void GeodeticDatum::set10TransformationParameters(double x, double z, double y, double rx, double ry, double rz, double scale, Coordinate center){
    set7TransformationParameters(x,y,z,rx,ry,rz,scale);
    _datumParams[dmCENTERXR] = center.x;
    _datumParams[dmCENTERXR] = center.y;
    _datumParams[dmCENTERXR] = center.z;
    code(QString("%1,%2,%3,%4").arg(code()).arg( center.x).arg( center.y).arg( center.z));

}

double GeodeticDatum::parameter(DatumParameters parm) const {
    return _datumParams[parm];
}

bool GeodeticDatum::isValid() const
{
    return true; //TODO:
}

void GeodeticDatum::fromCode(const QString &gcode)
{
    QSqlQuery stmt(kernel()->database());
    QString query = QString("Select * from datum where code='%1'").arg(gcode);

    if (stmt.exec(query)) {
        if ( stmt.next()) {
            QString area = stmt.value(stmt.record().indexOf("area")).toString();
            QString geocode = stmt.value(stmt.record().indexOf("code")).toString();
            double dx = stmt.value(stmt.record().indexOf("dx")).toDouble();
            double dy = stmt.value(stmt.record().indexOf("dy")).toDouble();
            double dz = stmt.value(stmt.record().indexOf("dz")).toDouble();
            setArea(area);
            code(geocode);
            setWktName(stmt.value(stmt.record().indexOf("wkt")).toString());
            set3TransformationParameters(dx, dy, dz);

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
    QString dat;
    QString indent = QString(" ").repeated(spaces);
    QString ending = spaces == 0 ? "" : "\n";
    if ( _wkt.size() > 0)
        dat = indent + "DATUM[\"" + _wkt + "," + ending;
    else{
        dat = indent + "DATUM[\"Unknown\"," + ending;
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






