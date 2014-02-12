#include <QHash>
#include <QString>
#include <QStringList>
#include "ilwis.h"
#include "Kernel_global.h"
#include "proj4parameters.h"

Proj4Parameters::Proj4Parameters( const QString& p) : _hasDatum(false)
{
    QString parms = p;
    if ( parms.indexOf("proj4=") == 0)
        parms = parms.right(parms.size() - 6);
    auto list = parms.split("+");
    foreach(QString parm, list) {
        auto kvp = parm.trimmed();
        if ( kvp == "")
            continue;
        auto pair = kvp.split("=");
        if ( pair.size() == 1)
            continue;
        if ( pair.front() == "towgs84") {
            parseShifts(pair.back());
        } else {
            if ( pair.front() == "datum") {
                _hasDatum = true;
                _keyvalues["datum"] = pair.back();
            }
            _keyvalues[pair.front()] = pair.back();
        }
    }
}

void Proj4Parameters::parseShifts(const QString& shifts) {
    QStringList parts = shifts.split(",");
    if ( parts.size() == 7) {
        _hasDatum = true;
        _keyvalues["dx"] = parts[0];
        _keyvalues["dy"] = parts[1];
        _keyvalues["dz"] = parts[2];
        _keyvalues["rx"] = parts[3];
        _keyvalues["ry"] = parts[4];
        _keyvalues["rz"] = parts[5];
        _keyvalues["dscale"] = parts[6];
    }

}

QString Proj4Parameters::datum() const {
    QString dat = sUNDEF;
    if ( _hasDatum) {
        if ( _keyvalues.find("dx") == _keyvalues.end()){
            return sUNDEF;
        }
        dat = _keyvalues["dx"] + "," + _keyvalues["dy"] + "," + _keyvalues["dz"];
        if ( _keyvalues["rx"] != sUNDEF){
            dat += "," + _keyvalues["rx"] + "," + _keyvalues["ry"] + "," + _keyvalues["rz"]+ "," + _keyvalues["dscale"];
        }
    }
    return dat;
}
QString Proj4Parameters::operator [](const QString &key) const
{
    auto iter =  _keyvalues.constFind(key);
    if ( iter != _keyvalues.end())
        return iter.value();
    if (key == "towgs84")
        return datum();
    return sUNDEF;
}

bool Proj4Parameters::hasDatum() const
{
    return _hasDatum;
}
