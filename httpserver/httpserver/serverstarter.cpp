/**
  @file
  @author Stefan Frings
*/

#include "serverstarter.h"
#include "httplistener.h"
#include "requestmapper.h"
#include "kernel.h"
#include "juliantime.h"
#include <QDir>

/** Name of this application */
#define APPNAME "Ilwis-Objects minimal HTTP server"

/** Publisher of this application */
#define ORGANISATION "52north"

/** Short description of this application */
#define DESCRIPTION "Supporting http server for Ilwis-objects"

/** The HTTP listener of the application */


ServerStarter::~ServerStarter()
{

}


void ServerStarter::start() {
    // Initialize the core application
    _listener.reset(new HttpListener(arguments(), new RequestMapper()));
    Ilwis::Time time = Ilwis::Time::now();
    auto txt =QString("ServiceHelper: Service has started at : %1").arg(time.toString()).toLatin1();
    qWarning(txt);
}

void ServerStarter::stop() {
    // Note that the stop method is not called when you terminate the application abnormally
    // by pressing Ctrl-C or when you kill it by the task manager of your operating system.

    // Deleting the listener here is optionally because QCoreApplication does it already.
    // However, QCoreApplication closes the logger at first, so we would not see the shutdown
    // debug messages, without the following line of code:
    _listener.reset(0);

    qWarning("ServiceHelper: Service has been stopped");
}


ServerStarter::ServerStarter(const std::map<QString, QVariant>& args)
    : QtService(args, APPNAME)
{
    setServiceDescription(DESCRIPTION);
    //setStartupType(AutoStartup);
}




