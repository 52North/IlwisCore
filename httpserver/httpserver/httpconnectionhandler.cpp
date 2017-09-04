/**
  @file
  @author Stefan Frings
*/

#include "kernel.h"
#include "ilwiscontext.h"
#include "httprequesthandler.h"
#include "httpconnectionhandler.h"
#include "httpresponse.h"
#include <QTimer>
#include <QCoreApplication>

using namespace Ilwis;

HttpConnectionHandler::HttpConnectionHandler(UPHTTPRequestHandler &requestHandler) :
    QThread(),
     _requestHandler(requestHandler)
{
    Q_ASSERT(requestHandler!=0);
    currentRequest=0;
    busy = false;
    // execute signals in my own thread
    moveToThread(this);
    socket.moveToThread(this);
    readTimer.moveToThread(this);
    connect(&socket, SIGNAL(readyRead()), SLOT(read()));
    connect(&socket, SIGNAL(disconnected()), SLOT(disconnected()));
    connect(&readTimer, SIGNAL(timeout()), SLOT(readTimeout()));
    readTimer.setSingleShot(true);
    qDebug("HttpConnectionHandler (%p): constructed", this);
    this->start();
}


HttpConnectionHandler::~HttpConnectionHandler() {
    socket.close();
    quit();
    wait();
    qDebug("HttpConnectionHandler (%p): destroyed", this);
}


void HttpConnectionHandler::run() {
    qDebug("HttpConnectionHandler (%p): thread started", this);
    try {
        exec();
    }
    catch (...) {
        qCritical("HttpConnectionHandler (%p): an uncatched exception occured in the thread",this);
    }
    qDebug("HttpConnectionHandler (%p): thread stopped", this);
}


void HttpConnectionHandler::handleConnection(tSocketDescriptor socketDescriptor) {
    qDebug("HttpConnectionHandler (%p): handle new connection", this);
    busy = true;
    Q_ASSERT(socket.isOpen()==false); // if not, then the handler is already busy

    //UGLY workaround - we need to clear writebuffer before reusing this socket
    //https://bugreports.qt-project.org/browse/QTBUG-28914
    socket.connectToHost("",0);
    socket.abort();

    if (!socket.setSocketDescriptor(socketDescriptor)) {
        qCritical("HttpConnectionHandler (%p): cannot initialize socket: %s", this,qPrintable(socket.errorString()));
        return;
    }
    // Start timer for read timeout
    int readTimeout= ilwisconfig("server-settings/read-timeout", 10000);
    readTimer.start(readTimeout);
    // delete previous request
    delete currentRequest;
    currentRequest=0;
}


bool HttpConnectionHandler::isBusy() {
    return busy;
}

void HttpConnectionHandler::setBusy() {
    this->busy = true;
}


void HttpConnectionHandler::readTimeout() {
    qDebug("HttpConnectionHandler (%p): read timeout occured",this);

    //Commented out because QWebView cannot handle this.
    //socket.write("HTTP/1.1 408 request timeout\r\nConnection: close\r\n\r\n408 request timeout\r\n");

    socket.disconnectFromHost();
    delete currentRequest;
    currentRequest=0;
}


void HttpConnectionHandler::disconnected() {
    qDebug("HttpConnectionHandler (%p): disconnected", this);
    socket.close();
    readTimer.stop();
    busy = false;
}

void HttpConnectionHandler::read() {
#ifdef SUPERVERBOSE
    qDebug("HttpConnectionHandler (%p): read input",this);
#endif

    // Create new HttpRequest object if necessary
    if (!currentRequest) {
        currentRequest=new HttpRequest();
    }

    // Collect data for the request object
    while (socket.bytesAvailable() && currentRequest->getStatus()!=HttpRequest::complete && currentRequest->getStatus()!=HttpRequest::abort) {
        currentRequest->readFromSocket(socket);
        if (currentRequest->getStatus()==HttpRequest::waitForBody) {
            // Restart timer for read timeout, otherwise it would
            // expire during large file uploads.
            int readTimeout=ilwisconfig("server-settings/read-timeout", 10000);
            readTimer.start(readTimeout);
        }
    }

    // If the request is aborted, return error message and close the connection
    if (currentRequest->getStatus()==HttpRequest::abort) {
        socket.write("HTTP/1.1 413 entity too large\r\nConnection: close\r\n\r\n413 Entity too large\r\n");
        socket.disconnectFromHost();
        delete currentRequest;
        currentRequest=0;
        return;
    }

    // If the request is complete, let the request mapper dispatch it
    if (currentRequest->getStatus()==HttpRequest::complete) {
        readTimer.stop();
        qDebug("HttpConnectionHandler (%p): received request",this);
        HttpResponse response(&socket);
        try {
            _requestHandler->service(*currentRequest, response);
        }
        catch (...) {
            qCritical("HttpConnectionHandler (%p): An uncatched exception occured in the request handler",this);
        }

        // Finalize sending the response if not already done
        if (!response.hasSentLastPart()) {
            response.write(QByteArray(),true);
        }
        // Close the connection after delivering the response, if requested
        if (QString::compare(currentRequest->getHeader("Connection"),"close",Qt::CaseInsensitive)==0) {
            socket.disconnectFromHost();
        }
        else {
            // Start timer for next request
            int readTimeout=ilwisconfig("server-settings/read-timeout", 10000);
            readTimer.start(readTimeout);
        }
        // Prepare for next request
        delete currentRequest;
        currentRequest=0;
    }
}
