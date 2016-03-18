#ifndef GIACIS_GLOBAL_H
#define GIACIS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GIACIS_LIBRARY)
#  define GIACISSHARED_EXPORT Q_DECL_EXPORT
#else
#  define GIACISSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GIACIS_GLOBAL_H
