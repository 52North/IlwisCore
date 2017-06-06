#ifndef LOCKER_H
#define LOCKER_H

#include <QDebug>
#include <map>
#include <QThread>
#include "kernel_global.h"

#include <mutex>

typedef std::map<QThread *, int> LockInfo;

KERNELSHARED_EXPORT void printlockmessage(const QString& prefix, const QString& message, const LockInfo& counts) ;
namespace Ilwis {
template<typename MutexType=std::recursive_mutex> class KERNELSHARED_EXPORT Locker
{
public:
    Locker(MutexType &mut) : _mutex(mut){
        mut.lock();
    }

#ifdef QT_DEBUG
    Locker(MutexType &mut, const QString& debugMessage) : _mutex(mut),_message(debugMessage){
        _lockcount[QThread::currentThread()]++;
        printlockmessage("entering", _message, _lockcount);
        _mutex.lock();
    }
    virtual ~Locker() {
        if (_message != ""){
            _lockcount[QThread::currentThread()]--;
            printlockmessage("leaving", _message, _lockcount);
        }
        _mutex.unlock();
    }

#else
    virtual ~Locker() {
        _mutex.unlock();
    }
#endif
private:
    MutexType& _mutex;
    QString _message;
    static LockInfo _lockcount;
};
#ifdef QT_DEBUG
template<class T> LockInfo Locker<T>::_lockcount;
#endif
}

#endif // LOCKER_H
