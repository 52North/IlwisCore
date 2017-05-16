#include <QHash>
#include <QString>
#include <QStringList>
#include <QDebug>
#include "ilwis.h"
#include "kernel_global.h"
#include "proj4parameters.h"

std::multimap<quint32, Proj4Def> Proj4Parameters::_lookup;
std::map<QString, QString> Proj4Parameters::_lookupByName;

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
        if ( pair.size() == 1) {
            if ( pair.front() == "south")
                _keyvalues["south"] = "Yes";
            continue;
        }
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
    if ( parts.size() == 3) {
        _hasDatum = true;
        _keyvalues["dx"] = parts[0];
        _keyvalues["dy"] = parts[1];
        _keyvalues["dz"] = parts[2];
    } else if ( parts.size() == 7) {
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
        dat = _keyvalues.at("dx") + "," + _keyvalues.at("dy") + "," + _keyvalues.at("dz");
        if ( _keyvalues.at("rx") != sUNDEF){
            dat += "," + _keyvalues.at("rx") + "," + _keyvalues.at("ry") + "," + _keyvalues.at("rz")+ "," + _keyvalues.at("dscale");
        }
    }
    return dat;
}
QString Proj4Parameters::operator [](const QString &key) const
{
    auto iter =  _keyvalues.find(key);
    if ( iter != _keyvalues.end())
        return (*iter).second;
    if (key == "towgs84")
        return datum();
    return sUNDEF;
}

bool Proj4Parameters::hasDatum() const
{
    return _hasDatum;
}

bool Proj4Parameters::operator==(const Proj4Parameters &parms) const
{
    for(auto parm : _keyvalues){
        if ( parm.first == "units" || parm.first == "no_defs")
            continue;
        auto iter = parms._keyvalues.find(parm.first);
        if ( iter == parms._keyvalues.end())
            return false;
        if ( parm.second != (*iter).second)
            return false;

    }
    return true;
}

Proj4Def Proj4Parameters::lookupByName(const QString& name){
    QString simpelName = simplifyName(name);
    auto iter = _lookupByName.find(simpelName);
    if ( iter != _lookupByName.end()){
        return lookupDefintion((*iter).second);
    }
    return Proj4Def();
}

Proj4Def Proj4Parameters::lookupDefintion(const QString& proj4Def) {
    quint32 hasv = Proj4Parameters::hash(proj4Def);
    auto iter = _lookup.find(hasv);
    if ( iter == _lookup.end())
        return Proj4Def();
    Proj4Parameters defIn(proj4Def);
    while(iter != _lookup.end() && (*iter).first == hasv){
        Proj4Parameters defCur((*iter).second._proj4def);
        if (defCur == defIn)
            return (*iter).second;
        ++iter;
    }
    return Proj4Def();
}

void Proj4Parameters::add2lookup(const QString& name, const QString& proj4def, const QString epsg){
    quint32 hnum = hash(proj4def);
    _lookup.insert(std::pair<quint32, Proj4Def>(hnum,{name, proj4def, epsg}));
    _lookupByName[simplifyName(name)] = proj4def;
}

QString Proj4Parameters::simplifyName(QString name){
    //name.remove(" ");
   // name.remove("-");
   // name.remove("/");
   // name.remove("\\");
    name.remove(QRegExp("[ -/\\\"]"));
    name = name.toLower();

    return name;
}

quint32 Proj4Parameters::hash(QString code){
    quint32 hasv = 0;
    code = code.toLower();
    code.remove("+no_defs");
    QRegExp rx("+units=* ");
    rx.setPatternSyntax(QRegExp::Wildcard);
    code.remove(rx);
    code.remove(" ");
    for(auto c : code){
        hasv += c.unicode();
    }
    return hasv;

}
