#include <QDebug>
#include <QThread>
#include "locker.h"

void printlockmessage(const QString &prefix, const QString& message, const LockInfo &counts)
{
    if (message != "contains"){
        qDebug() << prefix << " " << message <<" "<< QThread::currentThread();
//        for(auto& l : counts){
//            qDebug() << l.first << l.second;
//        }
    }
}
