#include "kernel.h"
#include "ilwiscontext.h"
#include "templatecache.h"
#include <QDateTime>
#include <QStringList>
#include <QSet>

using namespace Ilwis;

TemplateCache::TemplateCache(QObject* parent)
    :TemplateLoader(parent)
{
    cache.setMaxCost(ilwisconfig("server-settings/cache-size",1000000));
    cacheTimeout = ilwisconfig("server-settings/cache-time",60000);
    qDebug("TemplateCache: timeout=%i, size=%i",cacheTimeout,cache.maxCost());
}

QString TemplateCache::tryFile(QString localizedName) {
    qint64 now=QDateTime::currentMSecsSinceEpoch();
    // search in cache
    qDebug("TemplateCache: trying cached %s",qPrintable(localizedName));
    CacheEntry* entry=cache.object(localizedName);
    if (entry && (cacheTimeout==0 || entry->created>now-cacheTimeout)) {
        return entry->document;
    }
    // search on filesystem
    entry=new CacheEntry();
    entry->created=now;
    entry->document=TemplateLoader::tryFile(localizedName);
    // Store in cache even when the file did not exist, to remember that there is no such file
    cache.insert(localizedName,entry,entry->document.size());
    return entry->document;
}

