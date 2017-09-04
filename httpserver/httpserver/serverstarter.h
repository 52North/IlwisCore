/**
  @file
  @author Stefan Frings
*/

#ifndef STARTUP_H
#define STARTUP_H

#include <memory>
#include <QtCore/QCoreApplication>
#include <QStringList>
#include "httpserver/service/qtservice.h"

class HttpListener;

/**
  Helper class to install and run the application as a windows
  service.
*/
class ServerStarter : public QtService
{
public:

    /** Constructor */
    ServerStarter(const std::map<QString, QVariant> &args = std::map<QString, QVariant>());
    ~ServerStarter();

protected:

    /** Start the service */
    void start();

    /**
      Stop the service gracefully.
      @warning This method is not called when the program terminates
      abnormally, e.g. after a fatal error, or when killed from outside.
    */
    void stop();

private:
    std::unique_ptr<HttpListener>  _listener;

};

#endif // STARTUP_H
