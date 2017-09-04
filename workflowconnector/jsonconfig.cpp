#include <QtDebug>
#include <QString>
#include <QStringBuilder>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QUrl>
#include <QFileInfo>
#include "jsonconfig.h"

JsonConfig::JsonConfig() :
    _isValidSystem(false), _isValidUser(false)
{
}

bool JsonConfig::loadSystemConfig(const QString name)
{
    QString val;
    QFile file;
    file.setFileName(name);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonValue vvv = doc.object().value(QString("geoserver"));
    QJsonObject geo = vvv.toObject();
    QJsonValue v;
    v = geo["host"]; _geo_host = v.toString();                       // f.e:  130.89.221.193, or geoserver.utwente.nl
    v = geo["port"]; _geo_port = QString("%1").arg(v.toDouble());    // f.e:  85
    v = geo["SRS"]; _geo_srs = v.toString();                         // f.e:  EPSG:21036
    v = geo["Xmin"]; _geo_Xmin = v.toString();                       // f.e:  696275.4
    v = geo["Xmax"]; _geo_Xmax = v.toString();                       // f.e:  813775.4
    v = geo["Ymin"]; _geo_Ymin = v.toString();                       // f.e:  9797373.37
    v = geo["Ymax"]; _geo_Ymax = v.toString();                       // f.e:  9885123.37
    v = geo["WMS_version"]; _wms_version = v.toString();             // f.e: 1.1.1, or 1.3.0 (needed for correct order of bbox)
    v = geo["width"]; _wms_width = v.toString();                     // f.e:  800
    v = geo["height"];_wms_height = v.toString();                    // f.e:  600
    v = geo["output_type"]; _out_type = v.toString();                // f.e.: "Geotiff;.tif"
    QStringList parts = _out_type.split(';');
    if (parts.size() == 2) {
        _out_type = parts[0];
        _out_ext = parts[1];
    }
    QString dummy = _wms_version;
    dummy.remove(".");
    _version = dummy.toInt();
    if (_version > 100) _version /= 10;

    _isValidSystem = true;
    return _isValidSystem;
}

bool JsonConfig::loadUserConfig(const QString name)
{
    QString val;
    QFile file;
    file.setFileName(name);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject geo = doc.object().value(QString("geoserver")).toObject();
    _workspace = geo.value(QString("workspace")).toString();
    geo = doc.object().value(QString("datafolder")).toObject();
    _userMainFolder = geo["name"].toString();

    _isValidUser = true;
    return _isValidUser;
}

QString JsonConfig::getWMSGetMapURL(const QString layer, QString &layerName)
{
    if (!_isValidSystem)
        return layer;

    if (layer.length() == 0)
        return layer;

    QFileInfo fn(layer);
    layerName = fn.baseName();
    if (fn.absolutePath().length() == 0)
        layerName = layer;
    layerName.replace(QRegExp("[ \\.]"), "_");   // layer name for use in WMS server

    QString base = QString("http://") % _geo_host % QString(":") % _geo_port % QString("/geoserver/") % _workspace % QString("/wms");
    QString request = QString("service=WMS&version=%1&request=GetMap&").arg(_wms_version) %
                      QString("layers=%1:%2&").arg(_workspace).arg(layerName) %
                      QString("styles=&") %
                      getSRS() % "&" %
                      getBBox() % "&" %
                      QString("width=%1&height=%2&").arg(_wms_width).arg(_wms_height) %
                      QString("format=application/openlayers");

    return base % QString("?") % request;

}

QString JsonConfig::getLocalName(const QString localName) {
    if (localName.length() == 0)
        return localName;

    QFileInfo fi;
    QString name = localName % _out_ext;
    fi.setFile(QDir(_userMainFolder), name);

    return fi.absoluteFilePath();
}

QString JsonConfig::getSRS()
{
    return QString("SRS=") % _geo_srs;
}

QString JsonConfig::getBBox()
{
    if (_version < 13)
        return QString("BBOX=") % _geo_Xmin % "," % _geo_Ymin % "," % _geo_Xmax % "," % _geo_Ymax;

    else
        return QString("BBOX=") % _geo_Ymin % "," % _geo_Xmin % "," % _geo_Ymax % "," % _geo_Xmax;
}
