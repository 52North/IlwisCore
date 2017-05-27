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
    InternalDatabaseConnection db;
    QString stmt = QString("select %1 from dataformats where code='" + code + "'").arg(code);
    if ( connector != sUNDEF)
        stmt += " and connector='" + connector + "'";

    if (db.exec(stmt) && db.next()) {

    }

}

DataFormat::DataFormat(const QString& connector, const QString &code, const QString &longname,
                       const QString &extensions, const QString &access, IlwisTypes datatypes, const QString &description, const QString& parts)
{
    _properties[fpCODE] = code;
    _properties[fpNAME] = longname;
    _properties[fpDESCRIPTION] = description;
    _properties[fpEXTENSION] = extensions;
    _properties[fpCONNECTOR] = connector;
    _properties[fpDATATYPE] = datatypes;
    _properties[fpREADWRITE] = access;
    _properties[fpPARTS] = access;
    if (hasType(datatypes, itRASTER)){
        _properties[fpEXTENDEDTYPE] = itGEOREF | itDOMAIN | itCOORDSYSTEM;
    }

}

void DataFormat::setProps(InternalDatabaseConnection& db, const QString &code){
    _properties[fpCODE] = code;
    _properties[fpNAME] = set(db.value("name").toString());
    _properties[fpDESCRIPTION] = set(db.value("description").toString());
    _properties[fpEXTENSION] = set(db.value("extension"));
    _properties[fpCONNECTOR] = set(db.value("connector").toString());
    _properties[fpDATATYPE] = set(db.value("datatype").toULongLong());
    _properties[fpREADWRITE] = set(db.value("readwrite").toString());
    _properties[fpEXTENDEDTYPE] = set(db.value("extendedtype").toULongLong());
    _properties[fpPARTS] = set(db.value("parts").toString());
    _isValid = true;
}

std::multimap<QString, DataFormat> DataFormat::getSelectedBy(FormatProperties prop, const QString& selection){
    QString criterium;
    InternalDatabaseConnection db;
    QString stmt = QString("select * from dataformats where %1").arg(selection);
    std::multimap<QString, DataFormat> formats;
    if (db.exec(stmt)) {
        while(db.next()){
            QString code = db.value("code").toString();
            switch (prop){
            case fpCODE:
                criterium = code; break;
            case fpNAME:
                criterium = db.value("name").toString(); break;
            case fpEXTENSION:
                criterium = db.value("extension").toString(); break;
            case fpCONNECTOR:
                criterium = db.value("connector").toString(); break;
            default:
                ERROR2(ERR_OPERATION_NOTSUPPORTED2,"Property", "format selection");
                return std::multimap<QString, DataFormat>();
            }
            if ( criterium.size() == 0 || criterium == sUNDEF)
                continue;

            DataFormat format;
            format.setProps(db,code);
            formats.insert(std::pair<QString, DataFormat>(criterium,format));
        }

    }
    return formats;
}

QVariantList DataFormat::getFormatProperties(FormatProperties prop, IlwisTypes types, QString connector, QString code){
    QVariantList result;
    InternalDatabaseConnection db;
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
//        case fpCONTAINER:
//            field = "type"; break;
        case fpDATATYPE:
            field = "datatype"; break;
        case fpCONNECTOR:
            field = "connector"; break;
        case fpREADWRITE:
            field = "readwrite"; break;
        case fpEXTENDEDTYPE:
            field = "extendedtype"; break;
        case fpPARTS:
            field = "parts"; break;
    }

    QString stmt = QString("select %1 from dataformats where (datatype & %2) != 0").arg(field).arg(types);
    if ( code != sUNDEF)
        stmt += " and code='" + code + "'";
    if ( connector != sUNDEF)
        stmt += " and connector='" + connector + "'";

    if (db.exec(stmt)) {
        while(db.next()){
            QVariant var = db.value(0).toString();
            if ( var.type() == QVariant::String){
                QStringList parts = var.toString().split(",");
                for(QString part : parts) {
                    if ( part.size() > 0)
                        result += part;
                }
            }
        }

    }
    return result;
}

bool DataFormat::supports(DataFormat::FormatProperties fp, IlwisTypes tp, const QVariant &prop, const QString& connector)
{
    QVariantList props = DataFormat::getFormatProperties(fp, tp,connector);
    for(QVariant& p : props){
        if ( p == prop)
            return true;
    }
    return false;
}

bool DataFormat::setFormatInfo(const QString& path, const QString connector) {
    QFile file;
    file.setFileName(path);
    if (file.open(QIODevice::ReadOnly)) {
        QString settings = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(settings.toUtf8());
        if ( !doc.isNull()){
            InternalDatabaseConnection sqlPublic;
            QJsonObject obj = doc.object();
            QJsonValue formats = obj.value("Formats");
            if ( formats.isArray()){
                InternalDatabaseConnection db("BEGIN TRANSACTION");
                QJsonArray arrFormats = formats.toArray();
                for(auto iter = arrFormats.begin(); iter != arrFormats.end(); ++iter) {
                    auto jsonValue  = *iter;
                    if ( jsonValue.isObject()) {
                        QJsonObject objv = jsonValue.toObject();
                        QString code = objv.value("code").toString(sUNDEF);
                        QString name = objv.value("name").toString(sUNDEF);
                        QString desc = objv.value("description").toString(sUNDEF);
                        QString type = objv.value("type").toString(sUNDEF);
                        QString ext = objv.value("extension").toString(sUNDEF);
                        QString datatp = objv.value("datatypes").toString(sUNDEF);
                        QString fileparts = objv.value("parts").toString(sUNDEF);
                        quint64 ilwtype = itUNKNOWN;
                        QStringList parts = datatp.split(",");
                        for(QString tp : parts)
                            ilwtype |= IlwisObject::name2Type(tp);
                        QString rw = objv.value("readwrite").toString("r");
                        QString extt = objv.value("extendedtype").toString(sUNDEF);
                        quint64 exttypes = itUNKNOWN;
                        parts = extt.split(",");
                        for(QString tp : parts)
                            exttypes |= IlwisObject::name2Type(tp);

                        QString parms = QString("'%1','%2','%3','%4','%5',%6,'%7','%8',%9,'%10'").arg(code.toLower(),name,desc, ext,type).arg(ilwtype).arg(connector).arg(rw).arg(exttypes).arg(fileparts);
                        QString stmt = QString("INSERT INTO dataformats VALUES(%1)").arg(parms);
                        bool ok = sqlPublic.exec(stmt);
                        if (!ok) {
                            return kernel()->issues()->logSql(sqlPublic.lastError());
                        }
                    }

                }
                db.exec("COMMIT TRANSACTION");
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

bool DataFormat::store()
{
    InternalDatabaseConnection sqlPublic;
    IlwisTypes extTypes = _properties[fpDATATYPE].toULongLong() == itRASTER ? itCOORDSYSTEM | itGEOREF | itDOMAIN : itUNKNOWN;
    QString parms = QString("'%1','%2','%3','%4','file',%5,'%6','%7',%8,'%9'").arg(_properties[fpCODE].toString(),
                                                                            _properties[fpNAME].toString(),
                                                                            _properties[fpDESCRIPTION].toString(),
                                                                              _properties[fpEXTENSION].toString()).
                                                                            arg(_properties[fpDATATYPE].toULongLong()).
                                                                            arg(_properties[fpCONNECTOR].toString()).
                                                                            arg(_properties[fpREADWRITE].toString()).
                                                                            arg(extTypes).
                                                                            arg(_properties[fpPARTS].toString());

    QString stmt = QString("INSERT INTO dataformats VALUES(%1)").arg(parms);
    bool ok = sqlPublic.exec(stmt);
    if (!ok) {
        return kernel()->issues()->logSql(sqlPublic.lastError());
    }
    return true;
}

QVariant DataFormat::set(const QVariant &original) const
{
    if ( original.type() == QVariant::String){
        if ( original.toString() == "")
            return sUNDEF;
    }
    if ( original.type() == QVariant::ULongLong){
        bool ok;
        original.toULongLong(&ok);
        if (!ok )
            return itUNKNOWN;
    }
    return original;
}


