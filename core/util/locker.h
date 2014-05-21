#ifndef LOCKER_H
#define LOCKER_H

#include "kernel_global.h"

#include <mutex>

namespace Ilwis {
class KERNELSHARED_EXPORT Locker : public std::lock_guard<std::mutex>
{
public:
    Locker(std::mutex &mut);
};
}

#endif // LOCKER_H
