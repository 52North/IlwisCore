#include "kernel.h"
#include "ilwiscontext.h"
#include "httpconnectionhandlerpool.h"

using namespace Ilwis;

HttpConnectionHandlerPool::HttpConnectionHandlerPool(UPHTTPRequestHandler& requestHandler)
    : QObject(), _requestHandler(requestHandler)
{
    cleanupTimer.start(ilwisconfig("server-settings/cleanup-interval",1000));
    connect(&cleanupTimer, SIGNAL(timeout()), SLOT(cleanup()));
}


HttpConnectionHandlerPool::~HttpConnectionHandlerPool() {
    // delete all connection handlers and wait until their threads are closed
    foreach(HttpConnectionHandler* handler, pool) {
        delete handler;
    }
    qDebug("HttpConnectionHandlerPool (%p): destroyed", this);
}


HttpConnectionHandler* HttpConnectionHandlerPool::getConnectionHandler() {   
    HttpConnectionHandler* freeHandler=0;
    mutex.lock();
    // find a free handler in pool
    foreach(HttpConnectionHandler* handler, pool) {
        if (!handler->isBusy()) {
            freeHandler=handler;
            freeHandler->setBusy();
            break;
        }
    }
    // create a new handler, if necessary
    if (!freeHandler) {
        int maxConnectionHandlers=ilwisconfig("server-settings/max-threads",100);
        if (pool.count()<maxConnectionHandlers) {
            freeHandler=new HttpConnectionHandler(_requestHandler);
            freeHandler->setBusy();
            pool.append(freeHandler);
        }
    }
    mutex.unlock();
    return freeHandler;
}



void HttpConnectionHandlerPool::cleanup() {
    int maxIdleHandlers=ilwisconfig("server-settings/min-threads",1);
    int idleCounter=0;
    mutex.lock();
    foreach(HttpConnectionHandler* handler, pool) {
        if (!handler->isBusy()) {
            if (++idleCounter > maxIdleHandlers) {
                pool.removeOne(handler);                
                delete handler;
                qDebug("HttpConnectionHandlerPool: Removed connection handler (%p), pool size is now %i",handler,pool.size());
                break; // remove only one handler in each interval
            }
        }
    }
    mutex.unlock();
}
