#include <QRegExp>
#include <QUrlQuery>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSettings>
#include <QSqlField>
#include "identity.h"
#include "kernel.h"
#include "version.h"
#include "ilwisdata.h"
#include "oshelper.h"
#include "connectorinterface.h"
#include "ilwiscontext.h"
#include "mastercatalog.h"
#include "oshelper.h"
#include "mastercatalogcache.h"

using namespace Ilwis;

MasterCatalogCache::MasterCatalogCache()
{

}

void MasterCatalogCache::store() const
{
    QUrl url = context()->cacheLocation();
    QString filename = url.toLocalFile() + "/mastercatalog.dump";
    QFile cacheFile(filename);
    quint64 maxid = 0;
    if ( cacheFile.open(QFile::WriteOnly)){
        QDataStream stream(&cacheFile);
        QString query = QString("select * from mastercatalog where container not like 'ilwis:%'");
        InternalDatabaseConnection db(query);
        stream << maxid;
        stream << maxid;
        stream << kernel()->version()->cacheVersion;
        while( db.next()){
            QSqlRecord record = db.record();
            Resource resource(record);
            stream << _MAGIC; // this marker ensure that every resource begins at the expected place. if not the version of the cache is still incompatible ( probably someone forgot to up the version number)
            resource.store(stream);
            maxid = std::max(maxid, resource.id());
        }
        cacheFile.seek(0);
        stream << _MAGIC;
        stream << maxid;


    }

}

void MasterCatalogCache::deleteCache(QDataStream& stream)
{
    stream.device()->close();
    kernel()->issues()->log(TR("Incompatible version of the mastercatalog cache, deleting cache"), IssueObject::itWarning);
    QUrl url = context()->cacheLocation();
    QString filename = url.toLocalFile() + "/mastercatalog.dump";
    QFile::remove(filename);
    _hashes.clear();
}

void MasterCatalogCache::load()
{
    QUrl url = context()->cacheLocation();
    QString filename = url.toLocalFile() + "/mastercatalog.dump";
    QFile cacheFile(filename);
    if ( cacheFile.open(QFile::ReadOnly)){
        QDataStream stream(&cacheFile);
        quint64 baseid;
        quint64 magic;
        stream >> magic;
        // the magic number ensure that the last stream was closed correctly and that the dump is written correct
        if ( magic == _MAGIC){
            stream >> baseid;
            QString cacheVersion;
            stream >> cacheVersion;
            if ( cacheVersion != kernel()->version()->cacheVersion){
                deleteCache(stream);
                return;
            }
            while(!stream.atEnd()){
                Resource resource;
                quint64 magic;
                stream >> magic;
                if ( magic != _MAGIC){
                    deleteCache(stream);
                    return;
                }
                resource.load(stream);
                if ( resource.isValid()){
                    uint hash = ::qHash(OSHelper::neutralizeFileName(resource.url(true).toString()));
                    _hashes[hash].push_back(resource);
                }
            }

            Identity::setBaseId(baseid);
        }

    }
}

std::vector<Resource> MasterCatalogCache::find(const QUrl& url, Time modifiedtime){
    std::vector<Resource> resources;
    if ( _hashes.size() > 0){
        uint hash = ::qHash(OSHelper::neutralizeFileName(url.toString()));
        auto iter = _hashes.find(hash);
        if ( iter != _hashes.end()){

            for(const Resource& resource : (*iter).second){
                auto t1 =  modifiedtime;
                auto t2 =  Ilwis::Time(resource.modifiedTime());
                if ( t1 == t2)
                    resources.push_back(resource);
            }
        }
    }
    return resources;
}

