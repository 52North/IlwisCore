/**
  @file
  @author Stefan Frings
*/

#include "kernel.h"
#include "ilwiscontext.h"
#include "httplistener.h"
#include "httpconnectionhandler.h"
#include "httpconnectionhandlerpool.h"
#include <QCoreApplication>

using namespace Ilwis;

HttpListener::HttpListener(const std::map<QString, QVariant>& args, HttpRequestHandler* requestHandler)
    : QTcpServer(0), _requestHandler(requestHandler)
{
    // Reqister type of socketDescriptor for signal/slot handling
    qRegisterMetaType<tSocketDescriptor>("tSocketDescriptor");
    // Create connection handler pool
    pool=new HttpConnectionHandlerPool(_requestHandler);
    // Start listening

    int port;
    auto iter =  args.find("port");
    if ( iter == args.end())
        port=ilwisconfig("server-settings/port",8080);
    else
        port = iter->second.toUInt();

    listen(QHostAddress::Any, port);
    if (!isListening()) {
        qCritical("HttpListener: Cannot bind on port %i: %s",port,qPrintable(errorString()));
    }
    else {
        qDebug("HttpListener: Listening on port %i",port);
    }
}

HttpListener::~HttpListener() {
    close();
    qDebug("HttpListener: closed");
    delete pool;
    qDebug("HttpListener: destroyed");
}

void HttpListener::incomingConnection(tSocketDescriptor socketDescriptor) {
#ifdef SUPERVERBOSE
    qDebug("HttpListener: New connection");
#endif
    HttpConnectionHandler* freeHandler=pool->getConnectionHandler();

    // Let the handler process the new connection.
    if (freeHandler) {
        // The descriptor is passed via signal/slot because the handler lives in another
        // thread and cannot open the socket when called by another thread.
        connect(this,SIGNAL(handleConnection(tSocketDescriptor)),freeHandler,SLOT(handleConnection(tSocketDescriptor)));
        emit handleConnection(socketDescriptor);
        disconnect(this,SIGNAL(handleConnection(tSocketDescriptor)),freeHandler,SLOT(handleConnection(tSocketDescriptor)));
    }
    else {
        // Reject the connection
        qDebug("HttpListener: Too many incoming connections");
        QTcpSocket* socket=new QTcpSocket(this);
        socket->setSocketDescriptor(socketDescriptor);
        connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
        socket->write("HTTP/1.1 503 too many connections\r\nConnection: close\r\n\r\nToo many connections\r\n");
        socket->disconnectFromHost();
    }
}
