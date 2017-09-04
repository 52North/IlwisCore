#include <QThread>
#include <QTimer>
#include "serverthread.h"




ServerThread::ServerThread(QtServiceBasePrivate *service, QObject *parent) :
    QThread(parent),_service(service)
{
}
 void ServerThread::run() {
     QtServiceStarter1 starter(_service);
     QTimer::singleShot(0, &starter, SLOT(slotStart()));
     exec();
 }
