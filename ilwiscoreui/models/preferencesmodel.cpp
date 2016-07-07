#include "kernel.h"
#include <QDir>
#include <QDesktopServices>
#include "ilwisconfiguration.h"
#include "ilwiscontext.h"
#include "dataformat.h"
#include "preferencesmodel.h"

using namespace Ilwis;

QString PreferencesModel::preferedDataFormat(const QString &type)
{
    IlwisTypes ilwtype;
    ilwtype = IlwisObject::name2Type(type);
    if (ilwtype == itUNKNOWN)
        return QString();
    if ( hasType(ilwtype,itFEATURE )){
        ilwtype = itFEATURE;
    }
    QString expr = QString("(datatype & %1)!=0 and (readwrite='rc' or readwrite='rcu')").arg(ilwtype);
    QString formatCode = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/default-conversion-format-" + type,QString(sUNDEF));
    if ( formatCode == sUNDEF){
           switch (ilwtype){
        case itRASTER:
            formatCode = "rastercoverage";break;
        case itPOINT:
        case itLINE:
        case itPOLYGON:
        case itFEATURE:
            formatCode = "featurecoverage";break;
        case itTABLE:
            formatCode =  "table";break;
        case itDOMAIN:
            formatCode = "domain";break;
        case itCOORDSYSTEM:
            formatCode =  "coordinatesystem";break;
        case itGEOREF:
            formatCode =  "georeference"; break;
        default:
            formatCode = sUNDEF;
        }
        expr += " and connector='stream'";
    }

    std::multimap<QString, Ilwis::DataFormat>  formats = Ilwis::DataFormat::getSelectedBy(Ilwis::DataFormat::fpNAME, expr);
    for(auto &format : formats)    {
        if ( formatCode == format.second.property(Ilwis::DataFormat::fpCODE).toString())
            return format.second.property(Ilwis::DataFormat::fpNAME).toString();
    }
    return sUNDEF;
}

void PreferencesModel::setPreferedDataFormat(const QString &type, const QString &name)
{
    if ( name != "" && name != sUNDEF)
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/default-conversion-format-" + type,name);
}

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



