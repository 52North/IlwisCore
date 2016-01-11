#include <QRegExp>
#include <QUrlQuery>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSettings>
#include <QSqlField>
#include "identity.h"
#include "kernel.h"
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
        while( db.next()){
            QSqlRecord record = db.record();
            Resource resource(record);
            resource.store(stream);
            maxid = std::max(maxid, resource.id());
        }
        cacheFile.seek(0);
        stream << _MAGIC;
        stream << maxid;


    }

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
            while(!stream.atEnd()){
                Resource resource;
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
                if ( resource.modifiedTime() == modifiedtime)
                    resources.push_back(resource);
            }
        }
    }
    return resources;
}

