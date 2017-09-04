#ifndef WFSRESPONSE_H
#define WFSRESPONSE_H

#include <QObject>

#include "wfsconnector_global.h"

class QTimer;
class QVariant;
class QIODevice;
class QNetworkReply;
class QNetworkRequest;
class QXmlStreamReader;
class QNetworkAccessManager;

namespace Ilwis {
namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsResponse: public QObject
{
    Q_OBJECT

public:
    WfsResponse();
    WfsResponse(QIODevice *device);
    ~WfsResponse();

    /**
     * @brief hasFinished indicates if the request is still running or not (for async requests).
     * @return false if request is still pending and not (completely) received yet.
     */
    bool hasFinished() const;

    /**
     * Checks is service response is an exception message. Please note that for asyncronous
     * requests to check via hasFinished() if the response has already been received.
     *
     * @brief isException indicates if the service response is an exception.
     * @return true if request failed due to a service exception.
     */
    bool isException() const;

    /**
     * Parses an exception message from the exception details.
     *
     * @return an exception message.
     */
    QString parseException() const;

    /**
     * Starts performing request by means of the underlying network manager. The request
     * can be triggered synchronously or asynchronously. The request can timeout when no
     * response could be received after some time. Then cancelRequest() is called.
     *
     * @brief performRequest starts async network communication.
     * @param request the actual request to perform.
     * @param async true if request shall be aysncronous (the default), or false otherwise.
     * @return the response (though, request may still be running).
     */
    QNetworkReply *performRequest(QNetworkRequest &request, bool async=true);

    /**
     * @return the device which holds the content of the response.
     */
    QIODevice *device() const;

    /**
     * Sets the device holding the content of the response.
     *
     * @param device the device holding the response.
     */
    void setDevice(QIODevice *device);

public slots:

    /**
     * Cancels the running request.
     */
    void cancelRequest();

    /**
     * Reads the network reply to string content. When done, state is set to finish and
     * the network reply pointer is deleted later on.
     *
     * @brief readResponse reads the network reply and indicates that hasFinished == true.
     * @param reply the network reply created after a request has been sent.
     */
    void readResponse(QNetworkReply *reply);

private:
    QTimer *_connectionTimeout;
    QNetworkAccessManager *_networkManager;
    QNetworkReply *_reply ;
    QIODevice *_iodevice;
    bool _finished = false;

    /**
     * To be called from the constructors to initialize instance.
     */
    void initialize();

};


typedef QSharedPointer<WfsResponse> SPWfsResponse;


}
}

#endif // WFSRESPONSE_H
