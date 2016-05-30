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
template<typename MutexType=std::recursive_mutex> class KERNELSHARED_EXPORT Locker : public std::lock_guard<MutexType>
{
public:
    Locker(MutexType &mut) : std::lock_guard<MutexType>(mut){

    }

#ifdef QT_DEBUG
    Locker(MutexType &mut, const QString& debugMessage) : std::lock_guard<MutexType>(mut),_message(debugMessage){
        _lockcount[QThread::currentThread()]++;
        printlockmessage("entering", _message, _lockcount);
    }
    virtual ~Locker() {
        if (_message != ""){
            _lockcount[QThread::currentThread()]--;
            printlockmessage("leaving", _message, _lockcount);
        }
    }


private:
    QString _message;
    static LockInfo _lockcount;
#endif
};
#ifdef QT_DEBUG
template<class T> LockInfo Locker<T>::_lockcount;
#endif
}

#endif // LOCKER_H
