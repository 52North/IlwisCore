#include "locker.h"

using namespace Ilwis;

Locker::Locker(std::mutex& mut) : lock_guard<std::mutex>(mut)
{
}
