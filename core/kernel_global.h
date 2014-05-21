#ifndef KERNEL_GLOBAL_H
#define KERNEL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CORE_LIBRARY)
#  define KERNELSHARED_EXPORT Q_DECL_EXPORT
#else
#  define KERNELSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // KERNEL_GLOBAL_H
