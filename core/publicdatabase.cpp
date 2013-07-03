#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <functional>
#include "kernel.h"
#include "factory.h"
#include "abstractfactory.h"
#include "catalog.h"
#include "mastercatalog.h"
#include "ilwiscontext.h"
#include "errorobject.h"
#include "publicdatabase.h"

using namespace Ilwis;

PublicDatabase::PublicDatabase() {
}

PublicDatabase::PublicDatabase(const QSqlDatabase &db) : QSqlDatabase(db)
{
}

void PublicDatabase::prepare() {
    QSqlQuery sql(*this);

    QString stmt = "create table operationMetadata (operationId TEXT, name TEXT, description TEXT,icon TEXT, grouping TEXT)";
    sql.exec(stmt);

    stmt = "create table parameterMetadata (operationId TEXT,parameterName TEXT,parmorder INTEGER, isinput INTEGER,type INTEGER, \
           cardinalty INTEGER,flagged INTEGER,description TEXT,icon TEXT)";
    sql.exec(stmt);

    stmt = "create table aliasses (alias TEXT, code TEXT, type TEXT, source TEXT)";
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

    stmt = "create table projection (  code TEXT, name TEXT, wkt TEXT, authority TEXT, description TEXT)";
    doQuery(stmt, sql);

    stmt = "create table codes (  code TEXT, linkedtable TEXT)";
    doQuery(stmt, sql);

    stmt = "create table numericdomain (  code TEXT, minv REAL, maxv REAL, resolution REAL,resolution_strict INTEGER, range_strict INTEGER,unit TEXT, parent TEXT,description TEXT)";
    doQuery(stmt, sql);

    stmt = "create table mastercatalog \
            (\
                itemid INTEGER,\
                name TEXT collate nocase,\
                code TEXT ,\
                container TEXT collate nocase,\
                resource TEXT collate nocase, \
                type INTEGER, \
                extendedtype INTEGER, \
                size INTEGER, \
                dimensions TEXT \
               )";
    doQuery(stmt, sql);

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



    loadPublicTables();

    if ( kernel()->issues()->maxIssueLevel() == IssueObject::itCritical) {
        throw ErrorObject(TR("Critical errors found when initialzing Public database"));
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
    QSqlQuery db(kernel()->database());
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
    insertProj4Epsg(sqlPublic);
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
        if ( filename == "datums.csv")
            ok = fillDatumRecord(parts, sqlPublic);
        else if ( filename == "ellipsoids.csv")
            ok = fillEllipsoidRecord(parts, sqlPublic);
        else if (filename == "projections.csv")
            ok = fillProjectionRecord(parts,sqlPublic);
        else if ( filename == "numericdomains.csv")
            ok = fillValueDomainRecord(parts, sqlPublic);
        if (!ok)
            return ;
    }

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
        stmt = QString("INSERT INTO codes VALUES('ellipsoid_%1', 'ellipsoid')").arg(parts[0]);
        return doQuery(stmt, sqlPublic);
    }
    kernel()->issues()->log(TR(ERR_INVALID_RECORD_SIZE_IN).arg("ellipsoids.csv"),IssueObject::itCritical);
    return false;

}

bool PublicDatabase::fillProjectionRecord(const QStringList& parts, QSqlQuery &sqlPublic) {
    if ( parts.size() == 5) {
        auto parms = QString("'%1','%2','%3','%4','%5'").arg(parts[0],parts[1],parts[2], parts[3], parts[4]);
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

bool PublicDatabase::doQuery(QString& query, QSqlQuery &sqlPublic)
{
    bool ok = sqlPublic.exec(query);
    if (!ok) {
        kernel()->issues()->logSql(sqlPublic.lastError());
        return false;
    }
    return true;
}
