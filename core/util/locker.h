#ifndef LOCKER_H
#define LOCKER_H

#include "kernel_global.h"

#include <mutex>

namespace Ilwis {
template<typename MutexType=std::recursive_mutex> class KERNELSHARED_EXPORT Locker : public std::lock_guard<MutexType>
{
public:
    Locker(MutexType &mut) : std::lock_guard<MutexType>(mut){

    }
};
}

#endif // LOCKER_H
