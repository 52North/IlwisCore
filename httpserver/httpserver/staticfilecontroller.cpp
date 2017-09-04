/**
  @file
  @author Stefan Frings
*/

#include "kernel.h"
#include "ilwiscontext.h"
#include "staticfilecontroller.h"
#include <QFileInfo>
#include <QDir>
#include <QDateTime>

using namespace Ilwis;

StaticFileController::StaticFileController(QObject* parent)
    :HttpRequestHandler(parent)
{
    maxAge = ilwisconfig("server-settings/max-age", 6000);
    encoding = ilwisconfig("server-settings/encoding",QString("UTF-8"));
    docroot = ilwisconfig("server-settings/document-root-path",QString("."));

    qDebug("StaticFileController: docroot=%s, encoding=%s, maxAge=%i",qPrintable(docroot),qPrintable(encoding),maxAge);
    maxCachedFileSize=ilwisconfig("server-settings/max-cached-filesize",65536);
    cache.setMaxCost(ilwisconfig("server-settings/cache-size",1000000));
    cacheTimeout=ilwisconfig("server-settings/cache-time",60000);
    qDebug("StaticFileController: cache timeout=%i, size=%i",cacheTimeout,cache.maxCost());
}


void StaticFileController::service(HttpRequest& request, HttpResponse& response) {
    QByteArray path=request.getPath();
    // Check if we have the file in cache
    qint64 now=QDateTime::currentMSecsSinceEpoch();
    mutex.lock();
    CacheEntry* entry=cache.object(path);
    if (entry && (cacheTimeout==0 || entry->created>now-cacheTimeout)) {       
        QByteArray document=entry->document; //copy the cached document, because other threads may destroy the cached entry immediately after mutex unlock.
        QByteArray filename=entry->filename;
        mutex.unlock();
        qDebug("StaticFileController: Cache hit for %s",path.data());
        setContentType(filename,response);
        response.setHeader("Cache-Control","max-age="+QByteArray::number(maxAge/1000));
        response.write(document);
    }
    else {
        mutex.unlock();
        // The file is not in cache.
        qDebug("StaticFileController: Cache miss for %s",path.data());
        // Forbid access to files outside the docroot directory
        if (path.contains("/..")) {
            qWarning("StaticFileController: detected forbidden characters in path %s",path.data());
            response.setStatus(403,"forbidden");
            response.write("403 forbidden",true);
            return;
        }
        // If the filename is a directory, append index.html.
        QString resource = docroot+path;
        if (QFileInfo(resource).isDir()) {
            resource = context()->ilwisFolder().absoluteFilePath() + "/resources/index.html";
        }
        // Try to open the file
        QFile file(resource);
        qDebug("StaticFileController: Open file %s",qPrintable(file.fileName()));
        if (file.open(QIODevice::ReadOnly)) {
            setContentType(path,response);
            response.setHeader("Cache-Control","max-age="+QByteArray::number(maxAge/1000));
            if (file.size()<=maxCachedFileSize) {
                // Return the file content and store it also in the cache
                entry=new CacheEntry();
                while (!file.atEnd() && !file.error()) {
                    QByteArray buffer=file.read(65536);
                    response.write(buffer);
                    entry->document.append(buffer);
                }
                entry->created=now;
                entry->filename=path;
                mutex.lock();
                cache.insert(request.getPath(),entry,entry->document.size());
                mutex.unlock();
            }
            else {
                // Return the file content, do not store in cache
                while (!file.atEnd() && !file.error()) {
                    response.write(file.read(65536));
                }
            }
            file.close();
        }
        else {
            if (file.exists()) {
                qWarning("StaticFileController: Cannot open existing file %s for reading",qPrintable(file.fileName()));
                response.setStatus(403,"forbidden");
                response.write("403 forbidden",true);
            }
            else {
                response.setStatus(404,"not found");
                response.write("404 not found",true);
            }
        }
    }
}

HttpRequestHandler *StaticFileController::create()
{
    return new StaticFileController();
}

void StaticFileController::setContentType(QString fileName, HttpResponse& response) const {
    if (fileName.endsWith(".png")) {
        response.setHeader("Content-Type", "image/png");
    }
    else if (fileName.endsWith(".jpg")) {
        response.setHeader("Content-Type", "image/jpeg");
    }
    else if (fileName.endsWith(".gif")) {
        response.setHeader("Content-Type", "image/gif");
    }
    else if (fileName.endsWith(".pdf")) {
        response.setHeader("Content-Type", "application/pdf");
    }
    else if (fileName.endsWith(".txt")) {
        response.setHeader("Content-Type", qPrintable("text/plain; charset="+encoding));
    }
    else if (fileName.endsWith(".html") || fileName.endsWith(".htm")) {
        response.setHeader("Content-Type", qPrintable("text/html; charset="+encoding));
    }
    else if (fileName.endsWith(".css")) {
        response.setHeader("Content-Type", "text/css");
    }
    else if (fileName.endsWith(".js")) {
        response.setHeader("Content-Type", "text/javascript");
    }
    // Todo: add all of your content types
}
