#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QLibrary>
#include "kernel.h"
#include "ilwiscontext.h"
#include "supportlibraryloader.h"
#include "oshelper.h"

using namespace Ilwis;

SupportLibraryLoader::SupportLibraryLoader(const QFileInfo &configPath) : _configLocation(configPath)
{
    if ( !configPath.exists())
        return;

    QString operatingSystem = OSHelper::operatingSystem();
    QString path = configPath.absoluteFilePath();
    QFile file;
    file.setFileName(path);
    if (file.open(QIODevice::ReadOnly)) {
        QString settings = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(settings.toUtf8());
        if ( !doc.isNull()){
            QJsonObject obj = doc.object();
            QJsonValue osses = obj.value("OperatingSystem");
            if ( osses.isArray()){
                QJsonArray arrOsses = osses.toArray();
                for(auto jvalue : arrOsses) {
                    QJsonObject os = jvalue.toObject();
                    if ( os["name"].toString() == operatingSystem){
                        QJsonArray libraries = os["libraries"].toArray();
                        for(auto oslibsvalue : libraries){
                            QJsonObject osprops = oslibsvalue.toObject();
                            _order[osprops["order"].toDouble()] = osprops["library"].toString();
                        }
                        return;

                    }
                }
            }
        }
    }
}
QString SupportLibraryLoader::order2name(quint32 order) const{
    auto iter = _order.find(order);
    if ( iter != _order.end())
        return iter->second;
    return sUNDEF;
}

bool SupportLibraryLoader::isValid() const
{
    return _order.size() > 0;
}

void SupportLibraryLoader::loadLibraries() const{
    std::map<quint32, QString> order;
    QLibrary lib;
    bool ok = order.size() > 0;
    for(auto name : _order){
        QString path;
        if (QFileInfo(name.second).exists()) {
            path = name.second;
        } else {
            QFileInfo inf(_configLocation.absolutePath() + "/..");
            path = inf.absoluteFilePath() + "/" + name.second;
        }
        qDebug() << "loading " << path;
        qDebug() << "loading " << path;
        lib.setFileName(path);
        ok = lib.load();
        if ( !ok){
            ERROR2(ERR_COULD_NOT_LOAD_2, TR("name"), "connector");
        }
    }
}
