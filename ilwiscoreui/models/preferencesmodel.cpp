#include "kernel.h"
#include <QDir>
#include <QDesktopServices>
#include "ilwisconfiguration.h"
#include "ilwiscontext.h"
#include "preferencesmodel.h"

using namespace Ilwis;

PreferencesModel::PreferencesModel(QObject *parent) : QObject(parent)
{

}

QString PreferencesModel::cacheLocation() const
{
     QString location = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/cache-location",QString(sUNDEF));
     if ( location == sUNDEF || !QFileInfo(QUrl(location).toLocalFile()).exists()) {
        return QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)).toString();
     }
     return location;
}

void PreferencesModel::setCacheLocation(const QString &loc)
{
    QUrl url(loc);
    QFileInfo inf(url.toLocalFile());
    if ( inf.exists()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/cache-location",loc);
    }

}

QString PreferencesModel::internalCatalog() const
{
    QString location = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/internalcatalog-location",QString(sUNDEF));
    if ( location == sUNDEF || !QFileInfo(QUrl(location).toLocalFile()).exists()) {
       return QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/internalcatalog").toString();
    }
    return location;
}

void PreferencesModel::setInternalCatalog(const QString &loc)
{
    QUrl url(loc);
    QFileInfo inf(url.toLocalFile());
    if ( inf.exists()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/internalcatalog-location",loc);
    }
}



