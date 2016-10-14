#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <functional>
#include "kernel.h"
#include "ilwiscontext.h"
#include "errorobject.h"
#include "publicdatabase.h"
#include "proj4parameters.h"
#include "geometries.h"

using namespace Ilwis;

PublicDatabase::PublicDatabase() {
}

PublicDatabase::PublicDatabase(const QSqlDatabase &db) : QSqlDatabase(db)
{
}

void PublicDatabase::prepare() {
    exec("BEGIN IMMEDIATE TRANSACTION");
    QSqlQuery sql(*this);

    QString stmt = "create table operationMetadata (operationId TEXT, name TEXT, description TEXT,icon TEXT, grouping TEXT)";
    sql.exec(stmt);

    stmt = "create table parameterMetadata (operationId TEXT,parameterName TEXT,parmorder INTEGER, isinput INTEGER,type INTEGER, \
            cardinalty INTEGER,flagged INTEGER,description TEXT,icon TEXT)";
            sql.exec(stmt);

    stmt = "create table aliasses (alias TEXT, code TEXT, type TEXT, source TEXT)";
    sql.exec(stmt);

    stmt = "create table workspaces (workspaceid INTEGER, description TEXT)";
    sql.exec(stmt);

    stmt = "create table workspace (workspaceid INTEGER, itemid INTEGER)";
    sql.exec(stmt);

    stmt = "create table datum \
            (\
                code TEXT,\
                name TEXT,\
                area TEXT, \
                wkt TEXT, \
                authority TEXT, \
                ellipsoid TEXT, \
                dx REAL, \
                sx REAL, \
                dy REAL, \
                sy REAL, \
                dz REAL, \
                sz REAL, \
                north REAL, \
                south REAL, \
                west REAL, \
                east REAL, \
                rx REAL, \
                ry REAL, \
                rz REAL, \
                scale REAL, \
                source TEXT, \
                description TEXT \
                )";

            doQuery(stmt, sql);

    stmt = "create table ellipsoid (  code TEXT, name TEXT, wkt TEXT, authority TEXT, majoraxis REAL, invflattening REAL, source TEXT, description TEXT)";
    doQuery(stmt, sql);

    stmt = "create table projection (  code TEXT, name TEXT, wkt TEXT, authority TEXT, description TEXT,parameters TEXT)";
    doQuery(stmt, sql);

    stmt = "create table filters (  code TEXT, type TEXT, rows INTEGER, columns INTEGER,definition TEXT, gain REAL, description TEXT)";
    doQuery(stmt, sql);

    stmt = "create table codes (  code TEXT, linkedtable TEXT)";
    doQuery(stmt, sql);

    stmt = "create table dataformats (  code TEXT, name TEXT, description TEXT, extension TEXT,type TEXT,datatype INTEGER, connector TEXT, readwrite TEXT, extendedtype TEXT, parts TEXT)";
    doQuery(stmt, sql);

    stmt = "create table numericdomain (  code TEXT, minv REAL, maxv REAL, resolution REAL,resolution_strict INTEGER, range_strict INTEGER,unit TEXT, parent TEXT,description TEXT)";
    doQuery(stmt, sql);

    stmt = "create table representation (  code TEXT, relateddomain TEXT, representationtype TEXT, definition TEXT, description TEXT)";
    doQuery(stmt, sql);

    stmt = "create table itemdomain (  code TEXT, name TEXT,theme TEXT, domaintype TEXT, description TEXT)";
    doQuery(stmt, sql);

    stmt = "create table domainitems (  code TEXT, itemcode TEXT, itemname TEXT, itemdescription TEXT)";
    doQuery(stmt, sql);


    stmt = "create table mastercatalog \
            (\
                itemid INTEGER,\
                name TEXT collate nocase,\
                code TEXT ,\
                description TEXT ,\
                container TEXT collate nocase,\
                rawcontainer TEXT collate nocase,\
                resource TEXT collate nocase, \
                rawresource TEXT collate nocase, \
                urlquery TEXT collate nocase, \
                type INTEGER, \
                extendedtype INTEGER, \
                size INTEGER, \
                dimensions TEXT, \
                modifiedtime REAL, \
                createtime REAL \
                )";
            doQuery(stmt, sql);

    doQuery("CREATE INDEX mastercat_id ON mastercatalog(itemid)", sql);
    doQuery("CREATE INDEX mastercay_urltp ON mastercatalog(rawresource,type)", sql);
    doQuery("CREATE INDEX mastercay_rurltp ON mastercatalog(resource,type)", sql);

    stmt = "create table catalogitemproperties \
            (\
                propertyvalue TEXT, \
                propertyname TEXT, \
                itemid INTEGER \
                )";
            doQuery(stmt, sql) ;

    stmt = "CREATE TABLE projectedcsy ( \
            code TEXT NOT NULL PRIMARY KEY, \
            name TEXT NOT NULL, \
            proj_params TEXT NOT NULL)";
            doQuery(stmt, sql);

    stmt = "CREATE TABLE teritories ( \
            name TEXT NOT NULL PRIMARY KEY, \
            code TEXT, \
            officialname TEXT,\
            continent TEXT NOT NULL,\
            region TEXT, \
            longmin REAL, \
            latmin REAL, \
            longmax REAL, \
            latmax REAL,\
            type TEXT )";
            doQuery(stmt, sql);

    stmt = "create table epsgcodeswithlatlonaxesorder \
            (\
                code TEXT \
                )";
            doQuery(stmt, sql);



    loadPublicTables();

    exec("COMMIT TRANSACTION");

    if ( kernel()->issues()->maxIssueLevel() == IssueObject::itCritical) {
        exec("ROLLBACK TRANSACTION");
        throw ErrorObject(TR("Critical errors found when initializing Public database"));
    }

}

bool PublicDatabase::code2Record(const QString &code, const QString &table, QSqlRecord& rec)
{
    auto query = QString("Select * from %1 where code='%2'").arg(table, code);
    QSqlQuery db(*this);
    if (doQuery(query, db)) {
        if ( db.next()) {
            rec = db.record();
            return true;
        }
    }
    return false;
}

QString PublicDatabase::findAlias(const QString &name, const QString &type, const QString &nspace)
{
    InternalDatabaseConnection db;
    QString query = QString("Select code from aliasses where alias='%1' and type='%2' and source='%3'").arg(name).arg(type).arg(nspace);
    if ( db.exec(query)) {
        if ( db.next())
            return db.value(0).toString();
    }
    return sUNDEF;
}

void PublicDatabase::loadPublicTables() {
    QSqlQuery sqlPublic(*this);
    insertFile("datums.csv", sqlPublic);
    insertFile("ellipsoids.csv",sqlPublic);
    insertFile("projections.csv",sqlPublic);
    insertFile("numericdomains.csv",sqlPublic);
    insertFile("filters.csv",sqlPublic);
    insertFile("codes_with_latlon_order.csv",sqlPublic);
    insertFile("representations.csv", sqlPublic);
    insertFile("teritories.csv", sqlPublic);
    insertProj4Epsg(sqlPublic);
    insertItemDomains(sqlPublic);

    addRegionallEnvelopes();
}

void PublicDatabase::addRegionallEnvelopes() {
    InternalDatabaseConnection db;
    QString query = "Select * from teritories where type='country'";
    struct Info{
        Info(const QString& name="", const Envelope& env=Envelope()) : _continent(name),_env(env) {}
        QString _continent;
        Envelope _env;
    };

    std::map<QString, Info> regions;
    if ( db.exec(query)) {
        while ( db.next()){
            Envelope env(Coordinate(db.value(5).toDouble(), db.value(6).toDouble()),
                         Coordinate(db.value(7).toDouble(),db.value(8).toDouble()));
            if ( env.isValid() && !env.isNull()){
                Info& old = regions[db.value(4).toString()];
                old._continent = db.value(3).toString();
                old._env += env;
            }
        }
    }
    for(auto& region : regions){
        QString parms = QString("'%1','%2','%3','%4','%5',%6,%7,%8,%9,'%10'").arg(region.first).arg(region.first).arg("")
                .arg(region.second._continent).arg("").arg(region.second._env.min_corner().x).arg(region.second._env.min_corner().y)
                .arg(region.second._env.max_corner().x).arg(region.second._env.max_corner().y).arg("region");
        QString stmt = QString("INSERT INTO teritories VALUES(%1)").arg(parms);
        if(!db.exec(stmt)) {
            kernel()->issues()->log(TR("Possible illegal records in teritories.csv at ") + region.first);
        }
    }

}

void PublicDatabase::insertItemDomains(QSqlQuery& itemdomaintable) {
    auto path = context()->ilwisFolder().absoluteFilePath() + "/resources/classifications";
    QStringList exts;
    exts << "*.json" << "*.JSON";
    QDir classificationdir(path);
    QFileInfoList files = classificationdir.entryInfoList(exts, QDir::Files);
    bool ok = itemdomaintable.prepare("INSERT INTO itemdomain VALUES(\
                                      :code,:name,:theme,:domaintype,:description )" );
    QSqlQuery domainitemstable(*this);
    QSqlQuery codetable(*this);
    ok = domainitemstable.prepare("INSERT INTO domainitems VALUES(:code,:itemcode,:itemname,:itemdescription)");
    for( auto file : files){
        QFile jsonfile;
        jsonfile.setFileName(file.absoluteFilePath());
        if (jsonfile.open(QIODevice::ReadOnly)) {
            QString classification = jsonfile.readAll();
            QJsonDocument doc = QJsonDocument::fromJson(classification.toUtf8());
            if ( !doc.isNull()){
                QJsonObject obj = doc.object();
                QString name = obj["name"].toString();
                QString code = obj["code"].toString();
                QString theme = obj["theme"].toString();
                QString descr = obj["description"].toString();
                QString dtype = obj["type"].toString();
                itemdomaintable.bindValue(":name", name);
                itemdomaintable.bindValue(":theme", theme);
                itemdomaintable.bindValue(":code", code);
                itemdomaintable.bindValue(":domaintype", dtype);
                itemdomaintable.bindValue(":description", descr);
                ok = itemdomaintable.exec();
                if (!ok) {
                    kernel()->issues()->logSql(itemdomaintable.lastError());
                    return;
                }
                QString stmt = QString("INSERT INTO codes VALUES('%1', 'itemdomain')").arg(code);
                doQuery(stmt, codetable);
                QJsonValue items = obj.value("items");
                if ( items.isArray()){
                    QJsonArray itemarray = items.toArray();
                    for(auto jvalue : itemarray) {
                        QJsonObject item = jvalue.toObject();
                        QString itemcode = item["code"].toString();
                        QString itemname = item["name"].toString();
                        QString itemdescription = item["description"].toString();
                        domainitemstable.bindValue(":itemcode", itemcode);
                        domainitemstable.bindValue(":itemname", itemname);
                        domainitemstable.bindValue(":itemdescription", itemdescription);
                        domainitemstable.bindValue(":code", code);
                        ok = domainitemstable.exec();
                        if (!ok) {
                            kernel()->issues()->logSql(domainitemstable.lastError());
                            return;
                        }
                    }
                }
            }else{
                kernel()->issues()->log(QString(TR("Invalid json item domain definition in file %1").arg(file.absoluteFilePath())),IssueObject::itWarning);
            }
        }
    }
}

void PublicDatabase::insertProj4Epsg(QSqlQuery& sqlPublic) {
    auto basePath = context()->ilwisFolder().absoluteFilePath() + "/resources";
    QFileInfo info(basePath + "/epsg.pcs");
    if ( !info.exists())
        return;

    QFile inifile(info.canonicalFilePath());
    if (!inifile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream textfile(&inifile);
    QString text = textfile.readAll();

    QStringList lines = text.split("\n");
    bool ok = sqlPublic.prepare("INSERT INTO projectedcsy VALUES(\
                                :code,:name,:proj_params )" );
            QString line, name;
    int i = 0;
    while( i < lines.size()) {
        line = lines[i];
        if ( line[0] == '#' && name == "")
            name = line.right(line.size() - 2);
        else if ( line[0] == '<') {
            int index = line.indexOf('>');
            QString epgsTxt = "epsg:" + line.mid(1,index - 1);
            QString projTxt = line.mid(index + 2, line.size() - 4 - index);
            projTxt = projTxt.trimmed();
            sqlPublic.bindValue(":code", epgsTxt);
            sqlPublic.bindValue(":name", name);
            sqlPublic.bindValue(":proj_params", projTxt);
            ok = sqlPublic.exec();
            if (!ok) {
                kernel()->issues()->logSql(sqlPublic.lastError());
                return;
            }
            Proj4Parameters::add2lookup(name,projTxt,epgsTxt);
            name = "";
        } else if ( line[0] == '#') { // comment line, skip it, next line will be empty
            ++i;
            name = "";
        }
        ++i;

    }
}

void PublicDatabase::insertFile(const QString& filename, QSqlQuery& sqlPublic) {
    auto basePath = context()->ilwisFolder().absoluteFilePath() + "/resources";
    QFileInfo fileInfo(basePath + "/" + filename);
    if ( !fileInfo.exists()) {
        kernel()->issues()->log(TR(ERR_MISSING_SYSTEM_FILE_1).arg(filename), IssueObject::itCritical);
        return ;
    }

    QFile file(fileInfo.absoluteFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        kernel()->issues()->log(TR(ERR_MISSING_SYSTEM_FILE_1).arg(filename), IssueObject::itCritical);
        return ;
    }

    QTextStream txtFile(&file);
    bool skip = true;
    while(!txtFile.atEnd()) {
        QString line = txtFile.readLine();
        if ( skip) { // first line is header
            skip = false;
            continue;
        }
        auto firstset = line.split(',');
        QStringList parts;
        foreach(auto s, firstset) {
            QString strimmed = s.trimmed();
            parts.push_back(strimmed);
        }

        bool ok = true;
        if ( parts.size() <= 1) {
            // skipping empty lines
            if (parts.size() == 0) {
                continue;
            }
            if (parts.size() == 1 && parts[0].isEmpty()) {
                continue;
            }
        }
        if ( filename == "datums.csv")
            ok = fillDatumRecord(parts, sqlPublic);
        else if ( filename == "ellipsoids.csv")
            ok = fillEllipsoidRecord(parts, sqlPublic);
        else if (filename == "projections.csv")
            ok = fillProjectionRecord(parts,sqlPublic);
        else if ( filename == "numericdomains.csv")
            ok = fillValueDomainRecord(parts, sqlPublic);
        else if ( filename == "filters.csv")
            ok = fillFiltersRecord(parts, sqlPublic);
        else if ( filename == "codes_with_latlon_order.csv")
            ok = fillEpsgWithLatLonAxesOrderRecord(parts, sqlPublic);
        else if ( filename == "representations.csv"){
            ok = fillRepresentationRecord(parts, sqlPublic);
        } else if ( filename == "teritories.csv"){
            ok = fillTeritoryRecord(parts, sqlPublic);
        }
        if (!ok)
            return ;
    }

}
bool PublicDatabase::fillTeritoryRecord(const QStringList& parts, QSqlQuery &sqlPublic){
    if ( parts.size() == 10) {
        QString parms = QString("'%1','%2','%3','%4','%5',%6,%7,%8,%9,'%10'").arg(parts[0],parts[1],parts[2], parts[3],parts[4],parts[5],parts[6],parts[7]).arg(parts[8],parts[9]);
        QString stmt = QString("INSERT INTO teritories VALUES(%1)").arg(parms);
        if(!doQuery(stmt, sqlPublic))
            return false;
        stmt = QString("INSERT INTO codes VALUES('%1', 'teritory')").arg(parts[1]);
        return doQuery(stmt, sqlPublic);
    }
    kernel()->issues()->log(TR(ERR_INVALID_RECORD_SIZE_IN).arg("representations.csv"),IssueObject::itCritical);
    return false;
}

bool PublicDatabase::fillRepresentationRecord(const QStringList& parts, QSqlQuery &sqlPublic) {
    if ( parts.size() == 5) {
        QString parms = QString("'%1','%2','%3','%4','%5'").arg(parts[0],parts[1],parts[2], parts[3],parts[4]);
        QString stmt = QString("INSERT INTO representation VALUES(%1)").arg(parms);
        if(!doQuery(stmt, sqlPublic))
            return false;
        stmt = QString("INSERT INTO codes VALUES('%1', 'representation')").arg(parts[0]);
        return doQuery(stmt, sqlPublic);
    }
    kernel()->issues()->log(TR(ERR_INVALID_RECORD_SIZE_IN).arg("representations.csv"),IssueObject::itCritical);
    return false;
}

bool PublicDatabase::fillValueDomainRecord(const QStringList& parts, QSqlQuery &sqlPublic) {
    if ( parts.size() == 9) {
        QString parms = QString("'%1',%2,%3,%4,%5,%6,'%7','%8','%9'").arg(parts[0],parts[1],parts[2], parts[3],parts[4],parts[5],parts[6],parts[7],parts[8]);
        QString stmt = QString("INSERT INTO numericdomain VALUES(%1)").arg(parms);
        if(!doQuery(stmt, sqlPublic))
            return false;
        stmt = QString("INSERT INTO codes VALUES('%1', 'numericdomain')").arg(parts[0]);
        return doQuery(stmt, sqlPublic);
    }
    kernel()->issues()->log(TR(ERR_INVALID_RECORD_SIZE_IN).arg("numericdomains.csv"),IssueObject::itCritical);
    return false;

}

bool PublicDatabase::fillEllipsoidRecord(const QStringList& parts, QSqlQuery &sqlPublic) {
    if ( parts.size() == 8) {
        QString parms = QString("'%1','%2','%3','%4',%5,%6,'%7','%8'").arg(parts[0],parts[1],parts[2], parts[3],parts[4],parts[5],parts[6],parts[7]);
        QString stmt = QString("INSERT INTO ellipsoid VALUES(%1)").arg(parms);
        if(!doQuery(stmt, sqlPublic))
            return false;
        stmt = QString("INSERT INTO codes VALUES('%1', 'ellipsoid')").arg(parts[0]);
        return doQuery(stmt, sqlPublic);
    }
    kernel()->issues()->log(TR(ERR_INVALID_RECORD_SIZE_IN).arg("ellipsoids.csv"),IssueObject::itCritical);
    return false;

}

bool PublicDatabase::fillFiltersRecord(const QStringList& parts, QSqlQuery &sqlPublic) {
    if ( parts.size() == 7) {
        auto parms = QString("'%1','%2',%3,%4,'%5',%6,'%7'").arg(parts[0],parts[1],parts[2], parts[3], parts[4],parts[5], parts[6]);
        auto stmt = QString("INSERT INTO filters VALUES(%1)").arg(parms);
        return doQuery(stmt, sqlPublic);
    }
    kernel()->issues()->log(TR(ERR_INVALID_RECORD_SIZE_IN).arg("filters.csv"),IssueObject::itCritical);
    return false;
}

bool PublicDatabase::fillEpsgWithLatLonAxesOrderRecord(const QStringList &parts, QSqlQuery &sqlPublic) {
    if (parts.size() == 1) {
        auto parms = QString("'%1'").arg(parts[0].toLower());
        auto stmt = QString ("INSERT INTO epsgcodeswithlatlonaxesorder VALUES(%1)").arg(parms);
        return doQuery(stmt, sqlPublic);
    }
    kernel()->issues()->log(TR(ERR_INVALID_RECORD_SIZE_IN).arg("codes_with_latlon_order.csv"), IssueObject::itCritical);
    return false;
}

bool PublicDatabase::fillProjectionRecord(const QStringList& parts, QSqlQuery &sqlPublic) {
    if ( parts.size() == 6) {
        auto parms = QString("'%1','%2','%3','%4','%5','%6'").arg(parts[0],parts[1],parts[2], parts[3], parts[4],parts[5]);
        auto stmt = QString("INSERT INTO projection VALUES(%1)").arg(parms);
        if(!doQuery(stmt, sqlPublic))
            return false;

        stmt = QString("INSERT INTO codes VALUES('%1', 'projection')").arg(parts[0]);
        return doQuery(stmt, sqlPublic);
    }
    kernel()->issues()->log(TR(ERR_INVALID_RECORD_SIZE_IN).arg("projections.csv"),IssueObject::itCritical);
    return false;

}

bool PublicDatabase::fillDatumRecord(const QStringList& parts, QSqlQuery &sqlPublic) {
    if ( parts.size() == 22) {
        QString parms = QString("'%1','%2','%3','%4','%5','%6',").arg(parts[0],parts[1],parts[2],parts[3],parts[4],parts[5]); //text parts
        parms += QString("%1,%2,%3,%4,%5,%6,").arg(parts[6],parts[7],parts[8],parts[9],parts[10],parts[11]); // shifts
        parms += QString("%1,%2,%3,%4,").arg(parts[12],parts[13],parts[14],parts[15]); //rotation
        parms += QString("%1,%2,%3,").arg(parts[16], parts[17], parts[18]); // nsew
        parms += QString("%1,'%2','%3'").arg(parts[19],parts[20], parts[21]); // scale, source,description
        QString stmt = QString("INSERT INTO datum VALUES(%1)").arg(parms);
        if(!doQuery(stmt, sqlPublic))
            return false;
        stmt = QString("INSERT INTO codes VALUES('%1', 'datum')").arg(parts[0]);
        return doQuery(stmt, sqlPublic);
    }
    kernel()->issues()->log(TR(ERR_INVALID_RECORD_SIZE_IN).arg("datums.csv"),IssueObject::itCritical);
    return false;
}



bool PublicDatabase::doQuery(const QString& query, QSqlQuery &sqlPublic)
{
    bool ok = sqlPublic.exec(query);
    if (!ok) {
        kernel()->issues()->logSql(sqlPublic.lastError());
        return false;
    }
    return true;
}


