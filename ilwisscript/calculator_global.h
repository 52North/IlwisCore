#ifndef CALCULATOR_GLOBAL_H
#define CALCULATOR_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CALCULATOR_LIBRARY)
#  define CALCULATORSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CALCULATORSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // CALCULATOR_GLOBAL_H
