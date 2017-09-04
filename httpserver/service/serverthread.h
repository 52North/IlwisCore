#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>
#include "qtservice.h"
#include "qtservice_p.h"

class QtServiceBasePrivate;

class ServerThread : public QThread
{
    Q_OBJECT
public:
    explicit ServerThread(QtServiceBasePrivate *service, QObject *parent = 0);


private:
    QtServiceBasePrivate *_service;
    void run() ;


};

class QtServiceStarter1 : public QObject
{
    Q_OBJECT
public:
    QtServiceStarter1(QtServiceBasePrivate *service,QObject *parent = 0)
        : QObject(parent), d_ptr(service) {}
public slots:
    void slotStart()
   {
       d_ptr->startService();
   }
private:
   QtServiceBasePrivate *d_ptr;
};

#endif // SERVERTHREAD_H
