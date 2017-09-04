#include <sstream>

#include <QTimer>
#include <QIODevice>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkSession>
#include <QNetworkConfiguration>
#include <QNetworkAccessManager>
#include <QXmlStreamReader>

#include "kernel.h"
#include "wfsresponse.h"
#include "xpathparser.h"
#include "xmlstreamparser.h"

using namespace Ilwis;
using namespace Wfs;

WfsResponse::WfsResponse() // TODO: rename to wfsconnection?!
{
    initialize();
    _finished = false;
}

WfsResponse::WfsResponse(QIODevice *device): _iodevice(device)
{
    initialize();
    _finished = true;
}

void WfsResponse::initialize()
{
    _connectionTimeout = new QTimer(this);
    _connectionTimeout->setSingleShot(true);
    connect(_connectionTimeout, SIGNAL(timeout()), this, SLOT(cancelRequest()));

    _networkManager = new QNetworkAccessManager(this);
}

WfsResponse::~WfsResponse()
{
    _iodevice->deleteLater();
    _networkManager->deleteLater();
    _connectionTimeout->deleteLater();
}

bool WfsResponse::hasFinished() const
{
    return _finished;
}

bool WfsResponse::isException() const
{
    if (!hasFinished()) {
        return false;
    }

    XmlStreamParser parser(device());
    parser.addNamespaceMapping("ows", "http://www.opengis.net/ows");

    return parser.startParsing("ows:ExceptionReport");
}

QString WfsResponse::parseException() const
{
    QString exceptionLog;
    XmlStreamParser parser(device());
    parser.addNamespaceMapping("ows", "http://www.opengis.net/ows");

    QXmlStreamReader *reader = parser.reader();
    if (parser.startParsing("ows:ExceptionReport")) {
        QString current = reader->name().toString();
        if (parser.findNextOf( {"ows:Exception"} )) {
            current = reader->name().toString();

            QString serverMsg;
            QXmlStreamAttributes attributes = parser.attributes();
            serverMsg.append(attributes.value("exceptionCode")).append(": ");
            serverMsg.append("locator '").append(attributes.value("locator")).append("' ");

            QString details("(");
            while (parser.findNextOf( {"ows:ExceptionText"} ))
            {
                current = reader->name().toString();
                details.append(parser.readElementText());
                details.append(", ");
            }
            details.remove(details.length() - 1);
            details.append(")");
            serverMsg.append(details);
            exceptionLog.append(serverMsg).append("\n");
        }
    }

    if (reader->hasError()) {
        QString error(reader->error());
        QString message(reader->errorString());
        ERROR2(TR(QString("Unable to parse exception.")).append(" (%1): %2"), error, message);
    }

    return exceptionLog;
}

QNetworkReply *WfsResponse::performRequest(QNetworkRequest &request, bool async)
{
    if (async) {
        QObject::connect(_networkManager, SIGNAL(finished(QNetworkReply*)),
                         this, SLOT(readResponse(QNetworkReply*)));
    }

    _reply = _networkManager->get(request);
    _connectionTimeout->start(30000);
    return _reply;
}

void WfsResponse::cancelRequest()
{
    ERROR0("The request will stop due to timeout. Check your internet connection.");
    _reply->abort();
}

QIODevice *WfsResponse::device() const
{
    return _iodevice;
}

void WfsResponse::setDevice(QIODevice *device)
{
    _iodevice = device;
    _finished = true;
}

void WfsResponse::readResponse(QNetworkReply *reply)
{
    _connectionTimeout->stop();
    if (reply->error() == QNetworkReply::NoError) {
        setDevice(reply);
    } else {
        QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        QVariant reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute);
        kernel()->issues()->log(TR("Error: %1)").arg(statusCode.toString()));
        kernel()->issues()->log(TR("Reason: %1)").arg(reason.toString()));
    }
}
