#include <QSqlQuery>
#include <QSqlError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "kernel.h"
#include "dataformat.h"

using namespace Ilwis;

DataFormat::DataFormat()
{
}

DataFormat::DataFormat(const QString &code, const QString connector)
{
    QSqlQuery db(kernel()->database());
    QString stmt = QString("select %1 from dataformats where code='" + code + "'").arg(code);
    if ( connector != sUNDEF)
        stmt += " and connector='" + connector + "'";

    if (db.exec(stmt) && db.next()) {
        _properties[fpCODE] = code;
        _properties[fpNAME] = set(db.value("name").toString());
        _properties[fpDESCRIPTION] = set(db.value("description").toString());
        _properties[fpEXTENSION] = set(db.value("extension"));
        _properties[fpCONTAINER] = set(db.value("container").toString());
        _properties[fpCONNECTOR] = set(db.value("connector").toString());
        _properties[fpDATATYPE] = set(db.value("datatype").toULongLong());
        _properties[fpREADWRITE] = set(db.value("readwrite").toString());
        _isValid = true;
    }

}

QStringList DataFormat::getFormatProperties(FormatProperties prop, IlwisTypes types, QString connector, QString code){
    QStringList result;
    QSqlQuery db(kernel()->database());
    QString field= "";
    switch( prop){
        case fpCODE:
            field = "code"; break;
        case fpNAME:
            field = "name"; break;
        case fpDESCRIPTION:
            field = "description"; break;
        case fpEXTENSION:
            field = "extension"; break;
        case fpCONTAINER:
            field = "type"; break;
        case fpDATATYPE:
            field = "datatype"; break;
        case fpCONNECTOR:
            field = "connector"; break;
    }

    QString stmt = QString("select %1 from dataformats where (datatype | %2) != 0").arg(field).arg(types);
    if ( code != sUNDEF)
        stmt += " and code='" + code + "'";
    if ( connector != sUNDEF)
        stmt += " and connector='" + connector + "'";

    if (db.exec(stmt)) {
        while(db.next()){
            QString val = db.value(0).toString();
            if ( val == sUNDEF)
                continue;
            QStringList parts = val.split(",");
            for(QString part : parts) {
                    result += part;

            }
        }

    }
    return result;
}

bool DataFormat::setFormatInfo(const QString& path, const QString connector) {
    QFile file;
    file.setFileName(path);
    if (file.open(QIODevice::ReadOnly)) {
        QString settings = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(settings.toUtf8());
        if ( !doc.isNull()){
            QSqlQuery sqlPublic(kernel()->database());
            QJsonObject obj = doc.object();
            QJsonValue formats = obj.value("Formats");
            if ( formats.isArray()){
                QJsonArray arrFormats = formats.toArray();
                for(auto iter = arrFormats.begin(); iter != arrFormats.end(); ++iter) {
                    auto jsonValue  = *iter;
                    if ( jsonValue.isObject()) {
                        QJsonObject objv = jsonValue.toObject();
                        QString code = objv.value("code").toString();
                        QString name = objv.value("name").toString();
                        QString desc = objv.value("description").toString();
                        QString type = objv.value("type").toString();
                        QString ext = objv.value("extension").toString();
                        QString datatp = objv.value("datatypes").toString();
                        quint64 ilwtype = itUNKNOWN;
                        QStringList parts = datatp.split(",");
                        for(QString tp : parts)
                            ilwtype |= IlwisObject::name2Type(tp);

                        QString parms = QString("'%1','%2','%3','%4','%5',%6,'%7'").arg(code,name,desc, ext,type).arg(ilwtype).arg(connector);
                        QString stmt = QString("INSERT INTO dataformats VALUES(%1)").arg(parms);
                        bool ok = sqlPublic.exec(stmt);
                        if (!ok) {
                            return kernel()->issues()->logSql(sqlPublic.lastError());
                        }
                    }

                }
                return true;
            }
        }

    }
    return false;
}

QVariant DataFormat::property(DataFormat::FormatProperties prop) const
{
    auto iter = _properties.find(prop);
    if (  iter != _properties.end())
        return (*iter).second;
    return QVariant();
}

bool DataFormat::isValid() const
{
    return _isValid;
}

QVariant DataFormat::set(const QVariant &original) const
{
    if ( original.type() == QMetaType::QString){
        if ( original.toString() == "")
            return sUNDEF;
    }
    if ( original.type() == QMetaType::ULongLong){
        bool ok;
        original.toULongLong(&ok);
        if (!ok )
            return itUNKNOWN;
    }
    return original;
}
